import { Router } from 'express'
import controller from '../controllers/classes'

const router = Router()

router.route('/')
    .get((req, res, next) => {
        controller.getClasses()
            .then(classes => res.status(200).send(classes))
            .finally(next)
    })
    .post((req, res, next) => {
        controller.createClass(req.body.name)
            .then(id => res.location(req.baseUrl + '/' + String(id)).status(201).send())
            .finally(next)
    })

router.route('/:id(\\d+)')
    .get((req, res, next) => {
        controller.getClass(req.params.id)
            .then(_class => res.status(200).send(_class))
            .catch(() => res.status(404).send())
            .finally(next)
    })
    .put((req, res, next) => {
        controller.updateClass(req.params.id, req.body.name)
            .then(() => res.status(204).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })
    .delete((req, res, next) => {
        controller.deleteClass(req.params.id)
            .then(() => res.status(204).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })

router.route('/:id(\\d+)/students')
    .get((req, res, next) => {
        controller.getStudents(req.params.id)
            .then(students => res.status(200).send(students))
            .finally(next)
    })
    .post((req, res, next) => {
        controller.addStudent(req.params.id, req.body.id)
            .then(() => res.status(201).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })

router.route('/:id(\\d+)/students/:student_id(\\d+)')
    .delete((req, res, next) => {
        controller.deleteStudent(req.params.id, req.params.student_id)
            .then(() => res.status(204).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })

export default router