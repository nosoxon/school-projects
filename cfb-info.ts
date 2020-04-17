import { stringify } from "querystring"

const CFB_MAGIC = [0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1]

function byteString(data: Buffer, offset: number, size: number): string {
    let s = ''
    for (let i = 0; i < size; ++i) {
        s += ('00' + data[offset+i].toString(16)).substr(-2) + ' '
    }
    return s.toUpperCase()
}

function padHex(data: number, padding: number): string {
    return ('0'.repeat(padding) + data.toString(16)).substr(-padding).toUpperCase()
}

function readBigUInt64LE(data: Buffer, offset: number = 0): BigInt {
    const first = data[offset];
    const last = data[offset + 7];
    if (first === undefined || last === undefined) {
      throw new Error('Out of bounds');
    }
  
    const lo = first +
      data[++offset] * 2 ** 8 +
      data[++offset] * 2 ** 16 +
      data[++offset] * 2 ** 24;
  
    const hi = data[++offset] +
      data[++offset] * 2 ** 8 +
      data[++offset] * 2 ** 16 +
      last * 2 ** 24;
  
    return BigInt(lo) + (BigInt(hi) << BigInt(32));
  }

const FREESECT  = 0xFFFFFFFF
const ENDCHAIN  = 0xFFFFFFFE
const FATSECT   = 0xFFFFFFFD
const DIFSECT   = 0xFFFFFFFC

export default async function(data: Buffer): Promise<Buffer> {
    console.log(`Header signature:     ${byteString(data, 0, 8)}`)
    console.log(`Header CLSID:         ${byteString(data, 8, 16)}`)
    console.log(`Minor version:        ${padHex(data.readUInt16LE(24), 4)}`)
    console.log(`Major version:        ${padHex(data.readUInt16LE(26), 4)}`)
    console.log(`Endianess:            ${padHex(data.readUInt16LE(28), 4)}`)
    const sectorShift = data.readUInt16LE(30)
    const sectorSize = 2 ** sectorShift
    console.log(`Sector shift:         ${padHex(sectorShift, 4)} (${sectorSize} bytes)`)
    console.log(`Mini sector shift:    ${padHex(data.readUInt16LE(32), 4)}`)
    console.log(`Reserved:             ${byteString(data, 34, 6)}`)
    console.log(`Directory sectors:    ${padHex(data.readUInt32LE(40), 8)}`)
    console.log(`FAT sectors:          ${padHex(data.readUInt32LE(44), 8)}`)
    const directoryEntry = data.readUInt32LE(48)
    console.log(`1st directory sector: ${padHex(directoryEntry, 8)}`)
    console.log(`TX signature #:       ${padHex(data.readUInt32LE(52), 8)}`)
    const ministreamCutoff = data.readUInt32LE(56)
    console.log(`Mini stream cutoff:   ${padHex(ministreamCutoff, 8)}`)
    const miniFatEntry = data.readUInt32LE(60)
    console.log(`1st mini FAT sector:  ${padHex(miniFatEntry, 8)}`)
    console.log(`Mini FAT sectors:     ${padHex(data.readUInt32LE(64), 8)}`)
    console.log(`1st DIFAT sector:     ${padHex(data.readUInt32LE(68), 8)}`)
    console.log(`DIFAT sectors:        ${padHex(data.readUInt32LE(72), 8)}`)
    console.log(`\nDIFAT Entries:`)
    let fatSectorNumbers: number[] = []
    for (let i = 0; i < 109; ++i) {
        const fatSectorLocation = data.readUInt32LE(76+4*i)
        if (fatSectorLocation !== FREESECT) {
            fatSectorNumbers.push(fatSectorLocation)
            console.log(`\t${i}\t${padHex(fatSectorLocation, 8)}`)
        }

        // MOAR
    }
    // If sector size > 512 bytes, then bytes in range 512-ss should be 0

    let FAT: { [index:number]: number } = {}
    console.log(`\nFAT Entries:`)
    for (let i = 0; i < fatSectorNumbers.length; ++i) {
        const offset = (fatSectorNumbers[i] + 1) * sectorSize
        //console.log(`offset: ${offset}`)
        for (let j = 0; j < sectorSize; j += 4) {
            const entryNumber = (i * sectorSize + j) / 4
            const entryData = data.readUInt32LE(offset+j)

            FAT[entryNumber] = entryData

            let info = ''
            switch (entryData) {
                case DIFSECT: info = 'DIFAT'; break
                case FATSECT: info = 'FAT'; break
                case ENDCHAIN: info = 'Chain end'; break
                case FREESECT: info = 'Free sector'; break
            }

            console.log(`\t${padHex(entryNumber, 8)}\t${padHex(entryData, 8)}\t${info}`)
        }
    }

    console.log(`\nMini FAT Entries:`)
    for (let i = miniFatEntry, n = 0; i !== ENDCHAIN; ++n, i = FAT[i]) {
        const offset = (i + 1) * sectorSize

        for (let j = 0; j < sectorSize; j += 4) {
            const entryNumber = (n * sectorSize + j) / 4
            const entryData = data.readUInt32LE(offset+j)

            console.log(`\t${padHex(entryNumber, 8)}\t${padHex(entryData, 8)}`)
        }
    }

    let ministreamEntry = 0

    console.log(`\nDirectory Entries:`)
    for (let i = directoryEntry, n = 0; i !== ENDCHAIN; ++n, i = FAT[i]) {
        const offset = (i + 1) * sectorSize

        for (let j = 0; j < sectorSize; j += 128) {
            const entry = data.slice(offset + j, offset + j + 128)
            const entryNumber = (n * sectorSize + j) / 128

            const objectType = entry.readUInt8(66)
            if (objectType === 0) {
                // set all to 0 except sibling + child IDs
                continue
            }

            console.log(`\tEntry ${entryNumber}`)

            const nameLength = entry.readUInt16LE(64)
            const name = entry.slice(0, nameLength-2).toString('utf16le')

            const objectTypes: {[index:number]: string} = {
                0: 'unknown',
                1: 'storage object',
                2: 'stream object',
                5: 'root storage object'
            }
            const objectTypeString = objectTypes[objectType] ? objectTypes[objectType] : 'bad value'
            const colorFlag = entry.readUInt8(67)
            const colorFlagString = colorFlag === 0 ? 'red' : 'black'
            const leftSiblingId = entry.readUInt32LE(68)
            const rightSiblingId = entry.readUInt32LE(72)
            const childId = entry.readUInt32LE(76)
            const CLSID = entry.slice(80, 96)
            const stateBits = entry.readUInt32LE(96)
            const creationTime = entry.slice(100, 108)
            const modifiedTime = entry.slice(108, 116)
            const startSector = entry.readUInt32LE(116)
            if (j === 0) ministreamEntry = startSector
            const streamSize = entry.slice(120, 128)
            const streamSizeN = readBigUInt64LE(entry, 120)

            console.log(`\t\tName:             ${name} (${name.length}) [${name[0]}]`)
            console.log(`\t\tType:             ${objectTypeString} (${objectType})`)
            console.log(`\t\tColor flag:       ${colorFlagString} (${colorFlag})`)
            console.log(`\t\tLeft sibling ID:  ${padHex(leftSiblingId, 8)}`)
            console.log(`\t\tRight sibling ID: ${padHex(rightSiblingId, 8)}`)
            console.log(`\t\tChild ID:         ${padHex(childId, 8)}`)
            console.log(`\t\tCLSID:            ${byteString(CLSID, 0, 16)}`)
            console.log(`\t\tState bits:       ${padHex(stateBits, 8)}`)
            console.log(`\t\tCreation time:    ${byteString(creationTime, 0, 8)}`)
            console.log(`\t\tModified time:    ${byteString(modifiedTime, 0, 8)}`)
            console.log(`\t\tStart sector:     ${padHex(startSector, 8)}`)
            console.log(`\t\tStream size:      ${byteString(streamSize, 0, 8)} (${streamSizeN})`)
            console.log()
        }
    }

    return Buffer.alloc(1)
}

