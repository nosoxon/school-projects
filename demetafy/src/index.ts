import fs from 'fs'

/*import gif from './formats/gif'

const imageData: Buffer = fs.readFileSync(process.argv[2])

gif(imageData).then(data => fs.writeFileSync('output.gif', data))*/

import cfb from './formats/microsoft/cfb'

const fileData: Buffer = fs.readFileSync(process.argv[2])

cfb(fileData)
