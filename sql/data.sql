-- 扫描任务表
-- CREATE TABLE scan_task (
--     `task_id`                           VARCHAR(255)    DEFAULT 0       NOT NULL,       -- 任务 ID
--     `scan_interval`                     TINYINT         DEFAULT 4       NOT NULL,       -- 变动扫描定时时长(默认 4 小时)
--     `scan_task_filter_name`             TEXT                            NOT NULL,       -- 扫描任务监测策略，以分号分割
--     `scan_task_dir`                     TEXT                            NOT NULL,       -- 扫描文件夹，以分号分割
--     `scan_task_dir_filterout`           TEXT                            NOT NULL,       -- 扫描例外文件夹，以分号分割
--     `scan_task_dir_filterout_fileext`   TEXT                            NOT NULL,       -- 扫描任务过滤的文件类型，以分号分割
--
--     `task_start_time`                   DATETIME        DEFAULT 0       NOT NULL,       -- 开始时间
--     `task_stop_time`                    DATETIME        DEFAULT 0       NOT NULL,       -- 结束时间
--     `task_file_count`                   INT4            DEFAULT 0       NOT NULL,       -- 终端总文件数
--     `task_scan_file_count`              INT4            DEFAULT 0       NOT NULL,       -- 需要扫描的文件总数
--     `task_scan_finished_file_count`     INT4            DEFAULT 0       NOT NULL,       -- 扫描完成的文件数
--
--     `task_status`                       TINYINT         DEFAULT 0       NOT NULL,       -- 任务状态：0 - 未知，1 - 进行中，2 - 暂停，3 - 停止
--     `scan_result_reuse`                 TINYINT         DEFAULT 0       NOT NULL,       -- 扫描结果复用：0 - 开启，1 - 关闭
--     `scan_task_file_monitor`            TINYINT         DEFAULT 0       NOT NULL,       -- 变动文件监控：0 - 开启，1 - 关闭
--     `scan_task_self_check`              TINYINT         DEFAULT 0       NOT NULL,       -- 自查任务：0 - 开启，1 - 关闭
--     PRIMARY KEY(task_id)
-- );


INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 001", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);

INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 002", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);

INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 003", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);

INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 004", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);

INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 005", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);

INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 006", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);

INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 007", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);

INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 008", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);

INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 009", "name1", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 1);


-- 扫描结果，仅保存被扫描任务命中的文件
-- CREATE TABLE scan_result (
--     `ID`                                INTEGER PRIMARY KEY AUTOINCREMENT,
--     `scan_file_name`                    TEXT                            NOT NULL,       -- 文件绝对路径
--     `policy_id`                         VARCHAR(32)                     NOT NULL,       -- 策略ID
--     `status`                            TINYINT         DEFAULT 0       NOT NULL,       -- 处理状态，0 - 未处理，5 - 删除，6 - 误报
--     `scan_finished_time`                DATETIME        DEFAULT 0       NOT NULL,       -- 扫描完成时间
--     `action_id`                         VARCHAR(32)                     NOT NULL,       -- 策略ID
--     `file_type`                         VARCHAR(8),                                     -- 文件类型
 --     `file_size`                         VARCHAR(16)                    NOT NULL,       -- 文件大小
--     `is_recognized`                     INTEGER         DEFAULT 1,                      -- 文件是否识别
--     `is_blocked`                        INTEGER         DEFAULT 1,                      -- 文件是否识别
--     `detect_result`                     TEXT            DEFAULT ''      NOT NULL,       -- 扫描结果
--     `status_reported`                   TINYINT         DEFAULT 0       NOT NULL,       -- 是否需要上报处理，0 - 不需要；1 - 需要
--     PRIMARY KEY(scan_file_name_md5,policy_id)
-- );


INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa", 'A', 0, 1658558157);

INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa1", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa2", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa3", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa4", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa5", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa6", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa7", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa8", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa9", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa00", 'A', 0, 1658558157);
INSERT INTO scan_result (scan_file_name, policy_id, status, scan_finished_time)
    VALUES ("/tmp/aa000", 'A', 0, 1658558157);

