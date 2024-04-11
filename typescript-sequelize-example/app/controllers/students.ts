import { Student, Class, Teacher } from '../models'

/* GET /students
 *   > Returns a list of all students */
async function getStudents(): Promise<any[]> {
    return await Student.findAll({
        attributes: ['id', 'first', 'last'],
        order: [['id', 'ASC']]
    })
}

/* POST /students
 *   first: the first name of the student
 *   last: the last name of the student
 *   > Creates a student with the parameters specified in the JSON body,
 *     returns a link to new student */
async function createStudent(first: string, last: string): Promise<number> {
    let student = await Student.create({
        first: first,
        last: last
    })

    return student.id
}

/* GET /students/:id
 *   > Returns details about the specified student */
async function getStudent(id: number): Promise<any> {
    let student = await Student.findByPk(id, {
        attributes: ['first', 'last']
    })

    if(!student)
        throw Error('404')
    
    return student
}

/* PUT /students/{id}
 *   first: the first name of the student
 *   last: the last name of the student
 *   > updates the specified student with the parameters passed in the JSON body */
async function updateStudent(id: number, first: string, last: string): Promise<void> {
    let student = await Student.findByPk(id)
    if(!student)
        throw Error('404')
    
    await student.update({
        first: first,
        last: last
    })
}

/* DELETE /students/{id}
 *   > Removes the specified student from the database */
async function deleteStudent(id: number): Promise<void> {
    let student = await Student.findByPk(id)
    if(!student)
        throw Error('404')

    await student.destroy()
}

/* GET /students/{id}/classes
 *   > Returns the classes the specified student is a part of */
async function getClasses(id: number): Promise<any[]> {
    return await Class.findAll({
        include: [{
            model: Student,
            where: { id: id },
            required: true,
            attributes: []
        }],
        attributes: ['id', 'name']
    })
}

/* POST /students/{id}/classes
 *   id: the id of the class to add to the student
 *   > Adds student to class specified in JSON body */
async function addClass(id: number, class_id: number): Promise<void> {
    let student = await Student.findByPk(id)
    let _class = await Class.findByPk(class_id)
    if(!student || !_class)
        throw Error('404')
    
    await student.addClass(_class)
}

/* DELETE /students/:id/classes
 *   > removes student from all classes */
async function deleteClasses(id: number): Promise<void> {
    let student = await Student.findByPk(id)
    if(!student)
        throw Error('404')

    await student.setClasses([])
}

/* DELETE /students/:id/classes/:class_id
 *   > removes student from specified class */
async function deleteClass(id: number, class_id: number): Promise<void> {
    let student = await Student.findByPk(id)
    let _class = await Class.findByPk(class_id)
    if(!student || !_class)
        throw Error('404')
    
    await student.removeClass(_class)
}

/* GET /students/:id/teachers
 *   > list each of a student's teachers */
async function getTeachers(id: number): Promise<any[]> {
    // Teachers aren't directly associated with students, so this
    // requires a complex query
    return await Teacher.findAll({
        include: [{
            model: Class,
            include: [{
                model: Student,
                where: { id: id },
                required: true,
                attributes: []
            }],
            required: true,
            attributes: []
        }],
        order: [['id', 'ASC']],
        attributes: ['id', 'first', 'last']
    })
}

export default {
    addClass: addClass,
    createStudent: createStudent,
    deleteClass: deleteClass,
    deleteClasses: deleteClasses,
    deleteStudent: deleteStudent,
    getClasses: getClasses,
    getStudent: getStudent,
    getStudents: getStudents,
    getTeachers: getTeachers,
    updateStudent: updateStudent
}