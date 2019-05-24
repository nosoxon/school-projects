import bodyparser from 'body-parser'
import express from 'express'

import router from './routes'

const server = express()

server.use(bodyparser.json())
server.use(router)

server.listen(5000)