export async function extract(data: Buffer, ranges: [number, number][]): Promise<Buffer> {
    // Remove any 0 length ranges
    const copyRanges = ranges.filter(v => v[0] !== v[1])
    // Calculate size of scrubbed data
    const scrubbedLength = ranges.reduce((p,c) => p + c[1] -c[0], 0)

    let scrubbedData = Buffer.alloc(scrubbedLength)
    let scrubbedDataOffset = 0
    copyRanges.forEach(range => {
        // Copy each range into the new buffer sequentially
        scrubbedDataOffset += data.copy(scrubbedData, scrubbedDataOffset, range[0], range[1])
    })

    return scrubbedData
}
