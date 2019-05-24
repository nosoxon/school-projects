import { Router } from 'express'
import controller from '../controllers/students'

const router = Router()

router.route('/')
    .get((req, res, next) => {
        controller.getStudents()
            .then(students => res.status(200).send(students))
            .finally(next)
    })
    .post((req, res, next) => {
        controller.createStudent(req.body.first, req.body.last)
            .then(id => res.location(req.baseUrl + '/' + String(id)).status(201).send())
            .finally(next)
    })

router.route('/:id(\\d+)')
    .get((req, res, next) => {
        controller.getStudent(req.params.id)
            .then(student => res.status(200).send(student))
            .catch(() => res.status(404).send())
            .finally(next)
    })
    .put((req, res, next) => {
        controller.updateStudent(req.params.id, req.body.first, req.body.last)
            .then(() => res.status(204).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })
    .delete((req, res, next) => {
        controller.deleteStudent(req.params.id)
            .then(() => res.status(200).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })

router.route('/:id(\\d+)/classes')
    .get((req, res, next) => {
        controller.getClasses(req.params.id)
            .then(classes => res.status(200).send(classes))
            .finally(next)
    })
    .post((req, res, next) => {
        controller.addClass(req.params.id, req.body.id)
            .then(() => res.location(
                    req.baseUrl + '/' + String(req.params.id) + '/classes/' + String(req.body.id)
                ).status(201).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })
    .delete((req, res, next) => {
        controller.deleteClasses(req.params.id)
            .then(() => res.status(200).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })

router.route('/:id(\\d+)/classes/:class_id(\\d+)')
    .delete((req, res, next) => {
        controller.deleteClass(req.params.id, req.params.class_id)
            .then(() => res.status(200).send())
            .catch(() => res.status(404).send())
            .finally(next)
    })

router.route('/:id(\\d+)/teachers')
    .get((req, res, next) => {
        controller.getTeachers(req.params.id)
            .then(teachers => res.send(teachers))
            .finally(next)
    })

export default router