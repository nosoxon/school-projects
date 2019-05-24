import { Router } from 'express'

import classRoutes from './classes'
import studentRoutes from './students'
import teacherRoutes from './teachers'

const router = Router()

router.use('/classes', classRoutes)
router.use('/students', studentRoutes)
router.use('/teachers', teacherRoutes)

export default router