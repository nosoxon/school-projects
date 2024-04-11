import { extract } from '../extract'

const PNG_MAGIC = [ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A ]

export default async function(data: Buffer): Promise<Buffer> {
    let ranges: [number, number][] = [[0, 8]]

    for (let i = 8; i < data.length;) {
        const length = data.readInt32BE(i)
        const type = data.slice(i + 4, i + 8).toString('ascii')

        switch (type) {
            case 'IHDR': case 'PLTE': case 'IDAT': case 'IEND':
                ranges.push([i, i + length + 12])
                break;
        }

        i += length + 12
    }

    return extract(data, ranges)
}
