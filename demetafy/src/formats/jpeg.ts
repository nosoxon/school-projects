import { extract } from '../extract'

const JPEG_MAGIC = 0xFFD8

export default async function(data: Buffer): Promise<Buffer> {
    // Check magic numbers    
    if (data.readUInt16BE(0) !== JPEG_MAGIC) {
        throw Error('not a jpeg')
    }

    // List of ranges to copy to the new JPEG
    let ranges: [number, number][] = []
    let start: number = 0
    let end: number | undefined = undefined

    for (let i = 0; i < data.length;) {
        if (data[i] === 0xFF) { // Start of marker
            const marker = data[i+1]
    
            if (marker === 0xFF || marker === 0x00) {
                ++i // not a marker
            } else if (marker == 0x01 || (marker >= 0xD0 && marker <= 0xD7)) {
                i += 2 // 2 byte segment
            } else if (marker == 0xD9) { // End of JPEG
                end = i + 2
                break
            } else { // Segment with length
                let length = 2 + data.readUInt16BE(i + 2)
    
                // Add a copy ranges around APPn and COM segments
                if (marker === 0xFE || (marker >= 0xE0 && marker <= 0xEF)) {
                    ranges.push([start, i])
                    start = i + length
                }

                i += length
            }
        } else {
            ++i
        }
    }

    if (!end) {
        throw Error('no JPEG terminator')
    }
    // Add the last range, from the end of the last metadata segment to the end of the JPEG data
    ranges.push([start, end])
    return extract(data, ranges)
}
