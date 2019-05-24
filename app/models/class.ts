import {
    Association, DataTypes, Model, Sequelize,
    BelongsToCreateAssociationMixin,
    BelongsToGetAssociationMixin,
    BelongsToSetAssociationMixin,
    BelongsToManyAddAssociationMixin,
    BelongsToManyAddAssociationsMixin,
    BelongsToManyCountAssociationsMixin,
    BelongsToManyCreateAssociationMixin,
    BelongsToManyGetAssociationsMixin,
    BelongsToManyHasAssociationMixin,
    BelongsToManyHasAssociationsMixin,
    BelongsToManyRemoveAssociationMixin,
    BelongsToManyRemoveAssociationsMixin,
    BelongsToManySetAssociationsMixin
} from 'sequelize'

import Student from './student'
import Teacher from './teacher'

class Class extends Model {
    public name!: string

    // Auto-generated
    public id!: number
    public createdAt!: Date
    public updatedAt!: Date

    // Student association methods
    public addStudent!: BelongsToManyAddAssociationMixin<Student, number>
    public addStudents!: BelongsToManyAddAssociationsMixin<Student, number>
    public countStudents!: BelongsToManyCountAssociationsMixin
    public createStudent!: BelongsToManyCreateAssociationMixin<Student>
    public getStudents!: BelongsToManyGetAssociationsMixin<Student>
    public hasStudent!: BelongsToManyHasAssociationMixin<Student, number>
    public hasStudents!: BelongsToManyHasAssociationsMixin<Student, number>
    public removeStudent!: BelongsToManyRemoveAssociationMixin<Student, number>
    public removeStudents!: BelongsToManyRemoveAssociationsMixin<Student, number>
    public setStudents!: BelongsToManySetAssociationsMixin<Student, number>

    // Teacher association methods
    public createTeacher!: BelongsToCreateAssociationMixin<Teacher>
    public getTeacher!: BelongsToGetAssociationMixin<Teacher>
    public setTeacher!: BelongsToSetAssociationMixin<Teacher, number>

    // Populated for inclusions
    public readonly Students?: Student[]
    public readonly Teacher?: Teacher

    public static associations: {
        Students: Association<Class, Student>
        Teacher: Association<Class, Teacher>
    }

    public static initialize(sequelize: Sequelize) {
        this.init({
            name: DataTypes.STRING
        }, {
            sequelize: sequelize,
            name: {
                singular: 'Class',
                plural: 'Classes'
            }
        })
    }
}

export default Class