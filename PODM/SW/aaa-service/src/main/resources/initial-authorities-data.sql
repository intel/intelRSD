INSERT INTO privilege values(nextval('hibernate_sequence'), 'Login');
INSERT INTO privilege values(nextval('hibernate_sequence'), 'ConfigureManager');
INSERT INTO privilege values(nextval('hibernate_sequence'), 'ConfigureUsers');
INSERT INTO privilege values(nextval('hibernate_sequence'), 'ConfigureComponents');
INSERT INTO privilege values(nextval('hibernate_sequence'), 'ConfigureSelf');

INSERT INTO roles values(nextval('hibernate_sequence'), 'Administrator');

INSERT INTO role_privileges values ((SELECT id from roles where role_name ='Administrator'), (SELECT id from privilege where name = 'Login'));
INSERT INTO role_privileges values ((SELECT id from roles where role_name ='Administrator'), (SELECT id from privilege where name = 'ConfigureManager'));
INSERT INTO role_privileges values ((SELECT id from roles where role_name ='Administrator'), (SELECT id from privilege where name = 'ConfigureUsers'));
INSERT INTO role_privileges values ((SELECT id from roles where role_name ='Administrator'), (SELECT id from privilege where name = 'ConfigureComponents'));
INSERT INTO role_privileges values ((SELECT id from roles where role_name ='Administrator'), (SELECT id from privilege where name = 'ConfigureSelf'));

INSERT INTO manager_account(id,password,username,role_id) values (nextval('hibernate_sequence'),'$2a$10$ve3OU0PgwPDtDhUcKClSTOl6QvvFXA/l9SBpywwl3HTYM/z523Qju', 'admin', (SELECT id from roles where role_name = 'Administrator'));