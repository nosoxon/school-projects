import {
    Association, DataTypes, Model, Sequelize,
    BelongsToManyAddAssociationMixin,
    BelongsToManyAddAssociationsMixin,
    BelongsToManyCountAssociationsMixin,
    BelongsToManyCreateAssociationMixin,
    BelongsToManyGetAssociationsMixin,
    BelongsToManyHasAssociationMixin,
    BelongsToManyHasAssociationsMixin,
    BelongsToManyRemoveAssociationMixin,
    BelongsToManyRemoveAssociationsMixin,
    BelongsToManySetAssociationsMixin,
} from 'sequelize'

import Class from './class'

class Student extends Model {
    public first!: string
    public last!: string

    // Auto-generated
    public id!: number
    public createdAt!: Date;
    public updatedAt!: Date;

    // Class association methods
    public addClass!: BelongsToManyAddAssociationMixin<Class, number>
    public addClasses!: BelongsToManyAddAssociationsMixin<Class, number>
    public countClasses!: BelongsToManyCountAssociationsMixin
    public createClass!: BelongsToManyCreateAssociationMixin<Class>
    public getClasses!: BelongsToManyGetAssociationsMixin<Class>
    public hasClass!: BelongsToManyHasAssociationMixin<Class, number>
    public hasClasses!: BelongsToManyHasAssociationsMixin<Class, number>
    public removeClass!: BelongsToManyRemoveAssociationMixin<Class, number>
    public removeClasses!: BelongsToManyRemoveAssociationsMixin<Class, number>
    public setClasses!: BelongsToManySetAssociationsMixin<Class, number>

    // Populated for inclusions
    public readonly Classes?: Class[]

    public static associations: {
        Classes: Association<Student, Class>
    }

    public static initialize(sequelize: Sequelize) {
        this.init({
            first: DataTypes.STRING,
            last: DataTypes.STRING
        }, { sequelize: sequelize })
    }
}

export default Student