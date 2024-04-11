import {
    Association, DataTypes, Model, Sequelize,
    HasManyAddAssociationMixin,
    HasManyAddAssociationsMixin,
    HasManyCountAssociationsMixin,
    HasManyCreateAssociationMixin,
    HasManyGetAssociationsMixin,
    HasManyHasAssociationMixin,
    HasManyHasAssociationsMixin,
    HasManyRemoveAssociationMixin,
    HasManyRemoveAssociationsMixin,
    HasManySetAssociationsMixin
} from 'sequelize'

import Class from './class'

class Teacher extends Model {
    public first!: string
    public last!: string

    // Auto-generated
    public id!: number
    public createdAt!: Date;
    public updatedAt!: Date;

    // Class association methods
    public addClass!: HasManyAddAssociationMixin<Class, number>
    public addClasses!: HasManyAddAssociationsMixin<Class, number>
    public countClasses!: HasManyCountAssociationsMixin
    public createClass!: HasManyCreateAssociationMixin<Class>
    public getClasses!: HasManyGetAssociationsMixin<Class>
    public hasClass!: HasManyHasAssociationMixin<Class, number>
    public hasClasses!: HasManyHasAssociationsMixin<Class, number>
    public removeClass!: HasManyRemoveAssociationMixin<Class, number>
    public removeClasses!: HasManyRemoveAssociationsMixin<Class, number>
    public setClasses!: HasManySetAssociationsMixin<Class, number>

    // Populated for inclusions
    public readonly classes?: Class[]

    public static associations: {
        classes: Association<Teacher, Class>
    }
    
    public static initialize(sequelize: Sequelize) {
        this.init({
            first: DataTypes.STRING,
            last: DataTypes.STRING
        }, { sequelize: sequelize })
    }
}

export default Teacher