/*病人*/
CREATE TABLE IF NOT EXISTS patient(
patient_id INT NOT NULL AUTO_INCREMENT,
patient_name VARCHAR(20) NOT NULL,
ID CHAR(20) NOT NULL,
patient_age INT NOT NULL,
patient_contact VARCHAR(60),
emergency_contact VARCHAR(60),
allergy_history VARCHAR(120),
PRIMARY KEY (patient_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 10000;
alter table patient add column patient_sex INT not null after patient_age;/*添加性别列*/
/*出厂商*/
CREATE TABLE IF NOT EXISTS factory(
factory_id INT NOT NULL AUTO_INCREMENT,
factory_name VARCHAR(20) NOT NULL,
factory_address VARCHAR(60),
factory_contact VARCHAR(60),
PRIMARY KEY (factory_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 10000;
/*药品*/
CREATE TABLE IF NOT EXISTS drug(
drug_id INT NOT NULL AUTO_INCREMENT,
drug_name VARCHAR(20) NOT NULL,
factory_id INT NOT NULL,
drug_buyprice FLOAT(8,2) NOT NULL,
drug_price FLOAT(8,2) NOT NULL,
real_inventory INT NOT NULL,
virtual_inventory INT NOT NULL,
PRIMARY KEY (drug_id),
FOREIGN KEY(factory_id) REFERENCES factory(factory_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 10000;
/*职位*/
CREATE TABLE IF NOT EXISTS positions(
position_id INT NOT NULL AUTO_INCREMENT,
position_name VARCHAR(20) NOT NULL,
PRIMARY KEY (position_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 100;
alter table positions add column position_limit INT NOT NULL;/*职位需要添加权限设置*/
/*科室*/
CREATE TABLE IF NOT EXISTS department(
department_id INT NOT NULL AUTO_INCREMENT,
department_name VARCHAR(12) NOT NULL,
department_info VARCHAR(120),
PRIMARY KEY (department_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 100;
/*添加唯一性约束*/
alter table department add unique(department_name);
/*员工*/
CREATE TABLE IF NOT EXISTS staff(
staff_id INT NOT NULL AUTO_INCREMENT,
staff_name VARCHAR(20) NOT NULL,
staff_sex VARCHAR(4) NOT NULL,
staff_age INT NOT NULL,
position_id INT NOT NULL,
staff_password VARCHAR(40) NOT NULL,
department_id INT NOT NULL,
PRIMARY KEY (staff_id),
FOREIGN KEY(position_id) REFERENCES positions(position_id),
FOREIGN KEY(department_id) REFERENCES department(department_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 10000;
alter table staff modify column staff_sex INT;/*员工性别用int表示*/
/*处方单*/
CREATE TABLE IF NOT EXISTS psp(
psp_id INT NOT NULL AUTO_INCREMENT,
doctor_id INT NOT NULL,
patient_id INT NOT NULL,
psp_date TEXT NOT NULL,
PRIMARY KEY (psp_id),
FOREIGN KEY(doctor_id) REFERENCES staff(staff_id),
FOREIGN KEY(patient_id) REFERENCES patient(patient_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 10000;
alter table psp add column patient_disease TEXT not null after patient_id;/*增加了诊断*/
/*药方详情*/
CREATE TABLE IF NOT EXISTS pspdetail(
psp_id INT NOT NULL,
drug_id INT NOT NULL,
drug_num INT NOT NULL,
psp_price FLOAT(8,2) NOT NULL,
psp_info TEXT ,
PRIMARY KEY (psp_id, drug_id),
FOREIGN KEY(psp_id) REFERENCES psp(psp_id),
FOREIGN KEY(drug_id) REFERENCES drug(drug_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*缴费单*/
CREATE TABLE IF NOT EXISTS payment(
payment_id INT NOT NULL AUTO_INCREMENT,
psp_id INT NOT NULL,
staff_id INT NOT NULL,
sum_price FLOAT(9,2) NOT NULL,
payment_date TEXT NOT NULL,
PRIMARY KEY (payment_id),
FOREIGN KEY(psp_id) REFERENCES psp(psp_id),
FOREIGN KEY(staff_id) REFERENCES staff(staff_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 100000;
/*科室详情*/
CREATE TABLE IF NOT EXISTS dpmdetail(
dpmdetail_id INT NOT NULL AUTO_INCREMENT,
department_id INT NOT NULL,
dpmdetail_name VARCHAR(20) NOT NULL,
dpmdetail_position VARCHAR(20) NOT NULL,
PRIMARY KEY (dpmdetail_id),
FOREIGN KEY(department_id) REFERENCES department(department_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 1000;
/*添加唯一性约束*/
alter table dpmdetail add unique(dpmdetail_name);
alter table dpmdetail add unique(dpmdetail_position);
/*值班详情*/
CREATE TABLE IF NOT EXISTS duty(
duty_id INT NOT NULL AUTO_INCREMENT,
stuff_id INT NOT NULL,
dpmdetail_id INT NOT NULL,
start_date TEXT NOT NULL,
end_date TEXT NOT NULL,
PRIMARY KEY (duty_id),
FOREIGN KEY(dpmdetail_id) REFERENCES dpmdetail(dpmdetail_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 100000;
/*修改staff命名，并添加外建*/
ALTER  TABLE duty CHANGE stuff_id staff_id INT;
alter table duty add foreign key(staff_id) REFERENCES staff(staff_id);
/*值班记录*/
CREATE TABLE IF NOT EXISTS dutyrecord(
dutyrecord_id INT NOT NULL AUTO_INCREMENT,
duty_id INT NOT NULL,
dutyrecord_status INT NOT NULL,
staff_id INT NOT NULL,
real_start_date TEXT NOT NULL,
real_end_date TEXT NOT NULL,
PRIMARY KEY (dutyrecord_id),
FOREIGN KEY(duty_id) REFERENCES duty(duty_id),
FOREIGN KEY(staff_id) REFERENCES staff(staff_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8,AUTO_INCREMENT = 100000;

