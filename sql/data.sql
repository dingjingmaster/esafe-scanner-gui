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


INSERT INTO scan_task (task_id, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 001", 004, 'A;B;C;D', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 0, 0, 0, 0);

INSERT INTO scan_task (task_id, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 002", 004, 'B', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 1, 1, 0, 0);

INSERT INTO scan_task (task_id, scan_interval, scan_task_filter_name, scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext,
    task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,
    task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)
    VALUES ("TASK 003", 004, 'C', "/home", ".local", "*.png", 1658558157, 1658558157, 1000, 600, 100, 1, 1, 0, 0);

-- 扫描结果，仅保存被扫描任务命中的文件
-- CREATE TABLE scan_result (
--     `scan_file_name_md5`                VARCHAR(255)                    NOT NULL,       -- 根据文件绝对路径生成的 MD5
--     `scan_file_name`                    TEXT                            NOT NULL,       -- 文件绝对路径
--     `filter_name`                       TEXT                            NOT NULL,       -- 扫描命中策略
--     `status`                            TINYINT         DEFAULT 0       NOT NULL,       -- 处理状态，0 - 未处理，5 - 删除，6 - 误报
--     `scan_finished_time`                DATETIME        DEFAULT 0       NOT NULL,       -- 扫描完成时间
--     `file_create_time`                  DATETIME        DEFAULT 0       NOT NULL,       -- 文件创建时间
--     `file_modify_time`                  DATETIME        DEFAULT 0       NOT NULL,       -- 文件修改时间
--     PRIMARY KEY(scan_file_name_md5,filter_name)
-- );

INSERT INTO scan_result (scan_file_name_md5, scan_file_name, filter_name, status, scan_finished_time, file_create_time, file_modify_time)
    VALUES ("file1 md5", "/tmp/aa", 'A', 0, 1658558157, 1658558157, 1658558157);

INSERT INTO scan_result (scan_file_name_md5, scan_file_name, filter_name, status, scan_finished_time, file_create_time, file_modify_time)
    VALUES ("file2 md5", "/tmp/bb", 'B', 0, 1658558157, 1658558157, 1658558157);

INSERT INTO scan_result (scan_file_name_md5, scan_file_name, filter_name, status, scan_finished_time, file_create_time, file_modify_time)
    VALUES ("file3 md5", "/tmp/cc", 'c', 0, 1658558157, 1658558157, 1658558157);
