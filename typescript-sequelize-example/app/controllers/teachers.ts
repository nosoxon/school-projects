import { Teacher, Class, Student } from '../models'

/* GET /teachers
 *   > return a list of all teachers */
async function getTeachers(): Promise<any[]> {
    return await Teacher.findAll({
        order: [['id', 'ASC']],
        attributes: ['id', 'first', 'last']
    })
}

/* POST /teachers
 *   first: first name
 *   last: last name
 *   > Creates a new teacher with the details specified in the JSON body */
async function createTeacher(first: string, last: string): Promise<number> {
    let teacher = await Teacher.create({
        first: first,
        last: last
    })

    return teacher.id
}

/* GET /teachers/{id}
 *   > Return details about the specified teacher */
async function getTeacher(id: number): Promise<any> {
    let teacher = await Teacher.findByPk(id, {
        attributes: ['first', 'last']
    })

    if(!teacher)
        throw Error('404')
    
    return teacher
}

/* PUT /teachers/{id}
 *   first: first name
 *   last: last name
 *   > Update the specified teacher */
async function updateTeacher(id: number, first: string, last: string): Promise<void> {
    let teacher = await Teacher.findByPk(id)
    if(!teacher)
        throw Error('404')
    
    await teacher.update({
        first: first,
        last: last
    })
}

/* DELETE /teachers/{id}
 *   > remove the specified teacher */
async function deleteTeacher(id: number): Promise<void> {
    let teacher = await Teacher.findByPk(id)
    if(!teacher)
        throw Error('404')
    
    await teacher.destroy()
}

/* GET /teachers/{id}/classes
 * List all the classes a teacher teaches */
async function getClasses(id: number): Promise<any[]> {
    return await Class.findAll({
        include: [{
            model: Teacher,
            where: { id: id },
            required: true,
            attributes: []
        }],
        order: [['id', 'ASC']],
        attributes: ['id', 'name']
    })
}

/* GET /teachers/{id}/students
 *   > List all the students of the specified teacher */
async function getStudents(id: number): Promise<any[]> {
    return await Student.findAll({
        include: [{
            model: Class,
            include: [{
                model: Teacher,
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
    createTeacher: createTeacher,
    deleteTeacher: deleteTeacher,
    getClasses: getClasses,
    getStudents: getStudents,
    getTeacher: getTeacher,
    getTeachers: getTeachers,
    updateTeacher: updateTeacher
}