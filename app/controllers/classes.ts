import { Class, Student } from '../models';

/* GET /classes
 *   > Returns a list of all classes */
async function getClasses(): Promise<any[]> {
    return await Class.findAll({
        order: [['id', 'ASC']],
        attributes: ['id', 'name']
    })
}

/* POST /classes
 *   name: the name of the class, in a JSON object
 *   > Creates new class and returns link to it   */
async function createClass(name: string): Promise<number> {
    let _class = await Class.create({
        name: name
    })

    return _class.id
}

/* GET /classes/{id}
 *   > Returns details about the specified class */
async function getClass(id: number): Promise<any> {
    let _class = await Class.findByPk(id, {
        attributes: ['name']
    })

    if(!_class)
        throw Error('404')
    
    return _class
}

/* PUT /classes/{id}
 *   name: the name of the class, in a JSON object
 *   > Updates the name of the specified class */
async function updateClass(id: number, name: string): Promise<void> {
    let _class = await Class.findByPk(id)
    if(!_class)
        throw Error('404')
    
    await _class.update({
        name: name
    })
}

/* DELETE /classes/{id}
 *   > Deletes the specified class */
async function deleteClass(id: number): Promise<void> {
    let _class = await Class.findByPk(id)
    if(!_class)
        throw Error('404')

    await _class.destroy()
}

/* GET /classes/{id}/students
 *   > List all students in the specified class */
async function getStudents(id: number): Promise<any[]> {
    return await Student.findAll({
        include: [{
            model: Class,
            where: { id: id },
            required: true,
            attributes: []
        }],
        order: [['id', 'ASC']],
        attributes: ['id', 'first', 'last']
    })
}

/* POST /classes/{id}/students
 *   id: the id of the student to add, in a JSON object
 *   > Adds the student passed in the body to the specified class */
async function addStudent(id: number, student_id: number): Promise<void> {
    let _class = await Class.findByPk(id)
    let student = await Student.findByPk(student_id)
    if(!_class || !student)
        throw Error('404')
    
    await _class.addStudent(student)
}

/* DELETE /classes/:id/students/:student_id
 *   > removes the specified student from the specified class */
async function deleteStudent(id: number, student_id: number): Promise<void> {
    let _class = await Class.findByPk(id)
    let student = await Student.findByPk(student_id)
    if(!_class || !student)
        throw Error('404')
    
    await _class.removeStudent(student)
}

export default {
    addStudent: addStudent,
    createClass: createClass,
    deleteClass: deleteClass,
    deleteStudent: deleteStudent,
    getClass: getClass,
    getClasses: getClasses,
    getStudents: getStudents,
    updateClass: updateClass
}