/*病人*/
INSERT INTO patient(patient_name,ID,patient_age,patient_sex,patient_contact,emergency_contact,allergy_history) VALUES('小明','122199801020000',20,1,'110','119','小龙虾过敏');
INSERT INTO patient(patient_name,ID,patient_age,patient_sex,patient_contact,emergency_contact,allergy_history) VALUES('大明','122197801020000',40,1,'110','119','大龙虾过敏');
INSERT INTO patient(patient_name,ID,patient_age,patient_sex,patient_contact,emergency_contact,allergy_history) VALUES('老明','122195801020000',60,1,'110','119','老龙虾过敏');
INSERT INTO patient(patient_name,ID,patient_age,patient_sex,patient_contact,emergency_contact,allergy_history) VALUES('大芳','122197801020001',40,0,'110','119','大明过敏');
/*出厂商*/
INSERT INTO factory(factory_name,factory_address,factory_contact) VALUES('鸿茅药酒','内蒙古','110');
INSERT INTO factory(factory_name,factory_address,factory_contact) VALUES('泉建','天津','120');
INSERT INTO factory(factory_name,factory_address,factory_contact) VALUES('小林制药','日本','911');
/*药品*/
INSERT INTO drug(drug_id,drug_name,factory_id,drug_buyprice,drug_price,real_inventory,virtual_inventory) VALUES(1,'红毛大瓶',10000,20,30,1000,1000);
INSERT INTO drug(drug_id,drug_name,factory_id,drug_buyprice,drug_price,real_inventory,virtual_inventory) VALUES(2,'全建保健',10001,500,600,500,500);
INSERT INTO drug(drug_id,drug_name,factory_id,drug_buyprice,drug_price,real_inventory,virtual_inventory) VALUES(3,'小林创可贴',10002,20,30,100,100);
/*职位*/
INSERT INTO positions(position_name,position_limit) VALUES('院长',1);
INSERT INTO positions(position_name,position_limit) VALUES('医师',1);
INSERT INTO positions(position_name,position_limit) VALUES('护士长',0);
INSERT INTO positions(position_name,position_limit) VALUES('护士',2);
INSERT INTO positions(position_name,position_limit) VALUES('管理员',3);
/*科室*/
INSERT INTO department(department_name,department_info) VALUES('内科','普通门诊');
INSERT INTO department(department_name,department_info) VALUES('骨科','跌打损伤');
INSERT INTO department(department_name,department_info) VALUES('皮肤科','过敏炎症');
INSERT INTO department(department_name,department_info) VALUES('护士科','护士');
INSERT INTO department(department_name,department_info) VALUES('管理科','管理员工，设置信息');
/*员工*/
INSERT INTO staff(staff_name,staff_sex,staff_age,position_id,staff_password,department_id) VALUES('小王',1,28,101,'123',100);
INSERT INTO staff(staff_name,staff_sex,staff_age,position_id,staff_password,department_id) VALUES('老李',1,48,101,'123',101);
INSERT INTO staff(staff_name,staff_sex,staff_age,position_id,staff_password,department_id) VALUES('小赵',0,38,102,'123',103);
INSERT INTO staff(staff_name,staff_sex,staff_age,position_id,staff_password,department_id) VALUES('小黄',0,28,103,'123',103);
INSERT INTO staff(staff_name,staff_sex,staff_age,position_id,staff_password,department_id) VALUES('老白',1,30,104,'123',104);
/*处方单*/
INSERT INTO psp(psp_id,doctor_id,ID,patient_disease,psp_date) VALUES('2018123010302600',10000,'122199801020000','重度感冒','2018-12-30 10:30:26');
INSERT INTO psp(psp_id,doctor_id,ID,patient_disease,psp_date) VALUES('2018123110302600',10000,'122199801020000','重度感冒','2018-12-31 10:30:26');
/*药方详情*/
INSERT INTO pspdetail(psp_id,drug_id,drug_num,psp_price,psp_info) VALUES('2018123010302600',1,5,150,'每天一瓶');
INSERT INTO pspdetail(psp_id,drug_id,drug_num,psp_price,psp_info) VALUES('2018123010302600',2,5,3000,'每天一瓶');
/*缴费单*/
INSERT INTO payment(payment_id,psp_id,staff_id,sum_price,payment_date) VALUES('2018123010302600','2018123010302600',10003,3150,'2018-12-30 10:30:26');
/*科室详情*/
INSERT INTO dpmdetail(department_id,dpmdetail_name,dpmdetail_position) VALUES(100,'内科1','101室');
INSERT INTO dpmdetail(department_id,dpmdetail_name,dpmdetail_position) VALUES(100,'内科2','102室');
INSERT INTO dpmdetail(department_id,dpmdetail_name,dpmdetail_position) VALUES(101,'骨科','103室');
INSERT INTO dpmdetail(department_id,dpmdetail_name,dpmdetail_position) VALUES(102,'皮肤科','104室');
INSERT INTO dpmdetail(department_id,dpmdetail_name,dpmdetail_position) VALUES(103,'护士科','201室');
/*值班详情*/
INSERT INTO duty(staff_id,dpmdetail_id,start_date,end_date) VALUES(10001,1001,'2018-06-21 08:00:00 星期四','2018-06-21 11:50:00 星期四');
/*值班记录*/
INSERT INTO dutyrecord(duty_id,dutyrecord_status,staff_id,real_start_date,real_end_date) VALUES(100000,1,10001,'2018-06-21 08:05:06 星期四','2018-06-21 11:46:50 星期四');
