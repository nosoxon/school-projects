import { extract } from "../extract"

/* GIF Structure (https://www.w3.org/Graphics/GIF/spec-gif89a.txt) */

// TODO: add GIF87a
const GIF_MAGIC = [ 0x47, 0x49, 0x46, 0x38, 0x39, 0x61 ]

function subBlocksSize(data: Buffer, offset: number): number {
    let size = 0
    while (data[offset + size] !== 0x00) {
        size += 1 + data.readUInt8(offset + size)
    }

    return size + 1
}

export default async function(data: Buffer): Promise<Buffer> {
    // 18. Logical Screen Descriptor
    let startOffset = 13
    if (data[10] & 0x80) {
        startOffset += 3 * Math.pow(2, 1 + (data[10] & 0x07))
    }

    let ranges: [number, number][] = [[0, startOffset]]

    for (let i = startOffset; i < data.length;) {
        if (data[i] === 0x2C) { // 20. Image Descriptor
            let size = 11
            if (data[i+9] & 0x80) {
                size += 3 * Math.pow(2, 1 + (data[i+9] & 0x07))
            }

            size += subBlocksSize(data, i + size)
            ranges.push([i, i + size])
            i += size
        } else if (data[i] === 0x21) { // Extension block
            switch (data[i+1]) {
                case 0x01: { // Plain text
                    i += 13 + subBlocksSize(data, i + 13)
                    break
                } case 0xF9: { // Graphic control
                    ranges.push([i, i += 8])
                    break
                } case 0xFE: { // Comment
                    i += 2 + subBlocksSize(data, i + 2)
                    break
                } case 0xFF: { // Application
                    let size = 14 + subBlocksSize(data, i + 14)
                    let appId = data.slice(i + 3, i + 14).toString('ascii')
                    if (size === 19 && (appId === 'NETSCAPE2.0' || appId === 'ANIMEXTS1.0')) {
                        ranges.push([i, i + 19])
                    }
                    i += size
                    break
                }
            }
        } else if (data[i] === 0x3B) { // Trailer
            ranges.push([i, i + 1])
            break
        } else {
            throw Error('bad GIF')
        }
    }

    return extract(data, ranges)
}
