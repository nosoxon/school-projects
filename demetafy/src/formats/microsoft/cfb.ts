/* https://winprotocoldoc.blob.core.windows.net/productionwindowsarchives/MS-CFB/[MS-CFB].pdf */

import fs from 'fs'

const CFB_MAGIC = [0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1]

const FREESECT = 0xFFFFFFFF
const ENDCHAIN = 0xFFFFFFFE

interface DirectoryEntry {
    name: string
    sector: number
    size: number
    stream: boolean
    left: string
    right: string
    child: string
}

class CFB {
    private sectorSize: number
    private fatSectors: number[]
    private fat: number[]
    private miniFat: number[]
    private ministreamCutoff: number
    private ministreamSector: number
    public directory: DirectoryEntry[]

    constructor(private data: Buffer) {
        // TODO check magic, sane data length

        // 2.2 Sector Shift; sector size is 2^shift 
        this.sectorSize = 1 << data.readUInt16LE(30)
        // 2.2 Mini Stream Cutoff Size
        this.ministreamCutoff = data.readUInt32LE(56)

        this.fatSectors = this.getFatSectors()
        this.fat = this.getFat()
        this.miniFat = this.getMiniFat()
        this.directory = this.getDirectory()

        this.ministreamSector = this.directory[0].sector
    }

    private getSector(sector: number): Buffer {
        const offset = (sector + 1) * this.sectorSize
        return this.data.slice(offset, offset + this.sectorSize)
    }

    private getMiniSector(miniSector: number): Buffer {
        const streamOffset = miniSector * 64

        let sectorCount = ~~(streamOffset / this.sectorSize)
        let sector = this.ministreamSector
        while (sectorCount--) {
            sector = this.fat[sector]
        }

        const sectorOffset = streamOffset % this.sectorSize
        return this.getSector(sector).slice(sectorOffset, sectorOffset + 64)
    }

    /* DIFAT entries give the numbers of sectors containing the FAT */
    private getFatSectors(): number[] {
        let fatSectors: number[] = []
        // 2.2 Header DIFAT
        // First 109 DIFAT entries are contained in header
        for (let i = 76; i < 512; i += 4) {
            fatSectors.push(this.data.readUInt32LE(i))
        }

        // 2.2 First DIFAT Sector Location
        // Any over the first 109 are stored in chained sectors
        let next = this.data.readUInt32LE(68)
        while (next !== ENDCHAIN) {
            const sector = this.getSector(next)
            for (let i = 0; i < this.sectorSize; i += 4) {
                fatSectors.push(sector.readUInt32LE(i))
            }
            // Last entry in each sector is number of next DIFAT sector
            next = fatSectors.pop()!
        }

        // Remove everything starting from the first unallocated sector
        const endIndex = fatSectors.indexOf(FREESECT)
        return ~endIndex ? fatSectors.slice(0, endIndex) : fatSectors
    }

    /* FAT entries give the next sector for each sector (linked list) */
    private getFat(): number[] {
        let fat: number[] = []
        for (let i = 0; i < this.fatSectors.length; ++i) {
            const sector = this.getSector(this.fatSectors[i])
            for (let j = 0; j < this.sectorSize; j += 4) {
                fat.push(sector.readUInt32LE(j))
            }
        }

        return fat
    }

    /* Mini FAT represents 64 byte sectors rather than 512 or 4096 */
    private getMiniFat(): number[] {
        let miniFat: number[] = []
        // 2.2 First Mini FAT Sector Location
        for (let i = this.data.readUInt32LE(60); i !== ENDCHAIN; i = this.fat[i]) {
            const sector = this.getSector(i)
            for (let j = 0; j < this.sectorSize; j += 4) {
                miniFat.push(sector.readUInt32LE(j))
            }
        }

        return miniFat
    }

    private getDirectory(): DirectoryEntry[] {
        let directory: DirectoryEntry[] = []
        for (let i = this.data.readUInt32LE(48); i !== ENDCHAIN; i = this.fat[i]) {
            const sector = this.getSector(i)
            for (let j = 0; j < this.sectorSize; j += 128) {
                const entry = sector.slice(j, j + 128)
                const type = entry.readUInt8(66)
                
                if (type === 0) {
                    continue
                }

                const nameLength = entry.readUInt16LE(64)
                directory.push({
                    name: entry.slice(0, nameLength - 2).toString('utf16le'),
                    sector: entry.readUInt32LE(116),
                    //size: readBigUInt64LE(entry, 120),
                    size: entry.readUInt32LE(120), // In for trouble if stream is > 2GB
                    stream: type === 2,
                    left: entry.readUInt32LE(68).toString(16),
                    right: entry.readUInt32LE(72).toString(16),
                    child: entry.readUInt32LE(76).toString(16)
                })
            }
        }

        return directory
    }

    public getStream(entry: DirectoryEntry): Buffer | undefined {
        if (!entry.stream) {
            return undefined
        }

        const mini: boolean = entry.size < this.ministreamCutoff

        const fat = mini ? this.miniFat : this.fat
        const chunkSize = mini ? 64 : this.sectorSize
        const getChunk = (mini ? this.getMiniSector : this.getSector).bind(this)

        let stream = Buffer.alloc(entry.size)
        let copied = 0
        for (let i = entry.sector; i !== ENDCHAIN; i = fat[i]) {
            const sector = getChunk(i)
            const remaining = entry.size - copied
            if (remaining < chunkSize) {
                sector.copy(stream, copied, 0, remaining)
                break
            }

            sector.copy(stream, copied)
            copied += chunkSize
        }

        return stream
    }

    public clearStream(entry: DirectoryEntry): Buffer {
        const mini: boolean = entry.size < this.ministreamCutoff

        const fat = mini ? this.miniFat : this.fat
        const chunkSize = mini ? 64 : this.sectorSize
        const getChunk = (mini ? this.getMiniSector : this.getSector).bind(this)

        let copied = 0
        for (let i = entry.sector; i !== ENDCHAIN; i = fat[i]) {
            const sector = getChunk(i)
            const remaining = entry.size - copied
            if (remaining < chunkSize) {
                sector.fill(0, 0, remaining)
                break
            }

            sector.fill(0)
            copied += chunkSize
        }

        return this.data
    }
}

export default async function(data: Buffer): Promise<Buffer> {
    let mycfb = new CFB(data)
    for (let i = 0; i < mycfb.directory.length; ++i) {
        console.log(`${i.toString(16)}\t${JSON.stringify(mycfb.directory[i])}`)
    }
    //console.log(mycfb.directory)
    /*fs.writeFileSync('properties_stream.dat', mycfb.getStream(mycfb.directory[8]))
    fs.writeFileSync('SummaryInformation.dat', mycfb.getStream(mycfb.directory[10]))
    fs.writeFileSync('DocumentSummaryInformation.dat', mycfb.getStream(mycfb.directory[11]))*/

//    fs.writeFileSync('package_stream.dat', mycfb.getStream(mycfb.directory.find(e => e.name == 'package_stream')!))

    mycfb.directory.filter(e => e.stream).forEach(e => fs.writeFileSync(`streams/${e.name}.dat`, mycfb.getStream(e)))
    //mycfb.clearStream(mycfb.directory[10])
    //fs.writeFileSync('test.doc', mycfb.clearStream(mycfb.directory[11]))

    return Buffer.alloc(1)
}
