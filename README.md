# Meteorological Data Management Center

**Operating environment**:

> OS: CentOS 7 x86_64
> 
> RAM: 2GB
> 
> SCSI: 80GB
> 
> programming language: C/C++

**Project framework**:

![project](https://user-images.githubusercontent.com/88936287/225012442-0622ecbf-41f4-4a65-af28-c44b40aa1ae7.png)


**Detailed description of the project**: /Project Introduction Manual/Meteorological-Data-Management-Center.pdf


How do I start a project?

first:


`cd /project/idc1/c && vim start.sh`

Take the extraction data system as an example:

![image](https://user-images.githubusercontent.com/88936287/225015823-48d7ba99-08b9-4261-8e11-b8e423910368.png)

You need to change the required information to your own and register the table segments of the relevant database:

In this figure, 127.0.0.1 is the IP address, root is the username, 123456 is the password, mysql is the database type, and 3306 is the connection mysql port.

The basic table information of the database is shown in the figure:
![image](https://user-images.githubusercontent.com/88936287/225017220-2db33bff-2f6e-4647-bcf3-8362d515c330.png)
You need to create the information of the relevant table and insert the data.

The SQL statement path of the related table is: `\project\idc\sql`
The simulated data path for inserting SQL is: `\project\idc\ini\stcode.ini`

data format:
![image](https://user-images.githubusercontent.com/88936287/225019165-8d8ff9b7-6d36-46c9-be04-2f4799852f84.png)

You can then go to the /project/idc1/c path and use `sh start.sh`

> If you want to close the project, you can use `sh killall.sh`.

There may be other related operations that need to be changed, and if they exist and cannot be solved by themselves, you can raise an issue!!!


