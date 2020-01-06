import fs from 'fs'

import gif from './formats/gif'

const imageData: Buffer = fs.readFileSync(process.argv[2])

gif(imageData).then(data => fs.writeFileSync('output.gif', data))
