-- 扫描任务表
CREATE TABLE scan_task (
    `task_id`                           VARCHAR(255)    DEFAULT 0       NOT NULL,       -- 任务 ID
    `task_name`                         TEXT            DEFAULT ''      NOT NULL,       -- 任务 名称
    `scan_interval`                     TINYINT         DEFAULT 4       NOT NULL,       -- 变动扫描定时时长(默认 4 小时)
    `scan_task_filter_name`             TEXT                            NOT NULL,       -- 扫描任务监测策略，以分号分割
    `scan_task_dir`                     TEXT                            NOT NULL,       -- 扫描文件夹，以分号分割
    `scan_task_dir_filterout`           TEXT                            NOT NULL,       -- 扫描例外文件夹，以分号分割
    `scan_task_dir_filterout_fileext`   TEXT                            NOT NULL,       -- 扫描任务过滤的文件类型，以分号分割

    `task_start_time`                   DATETIME        DEFAULT 0       NOT NULL,       -- 开始时间 
    `task_stop_time`                    DATETIME        DEFAULT 0       NOT NULL,       -- 结束时间 
    `task_file_count`                   INT4            DEFAULT 0       NOT NULL,       -- 终端总文件数
    `task_scan_file_count`              INT4            DEFAULT 0       NOT NULL,       -- 需要扫描的文件总数
    `task_scan_finished_file_count`     INT4            DEFAULT 0       NOT NULL,       -- 扫描完成的文件数

    `task_status`                       TINYINT         DEFAULT 0       NOT NULL,       -- 任务状态：0 - 未开始，1 - 扫描中，2 - 已停止，3 - 已完成, 5 - 已暂停, 13 - 扫描发生错误
    `scan_result_reuse`                 TINYINT         DEFAULT 0       NOT NULL,       -- 扫描结果复用：0 - 开启，1 - 关闭
    `scan_task_file_monitor`            TINYINT         DEFAULT 0       NOT NULL,       -- 变动文件监控：0 - 开启，1 - 关闭
    `scan_task_self_check`              TINYINT         DEFAULT 0       NOT NULL,       -- 自查任务：1 - 开启，0 - 关闭
    PRIMARY KEY(task_id)
);


-- 中间数据
-- 扫描过程中 临时数据表(根据任务id 自建)
-- CREATE TABLE scan_task_detail_taskid (
--     `detail_file_name_md5`              VARCHAR(255)                    NOT NULL,       -- 扫描的文件名
--     `detail_file_name`                  TEXT                            NOT NULL,       -- 扫描文件路径
--     `detail_file_status`                TINYINT         DEFAULT 0       NOT NULL,       -- 文件是否扫描：0 - 未扫描，1 - 扫描完成
--     PRIMARY KEY(detail_file_name_md5)
-- );


-- 扫描结果，仅保存被扫描任务命中的文件
CREATE TABLE scan_result (
    `ID`                                INTEGER PRIMARY KEY AUTOINCREMENT,
    `scan_file_name`                    TEXT                            NOT NULL,       -- 文件绝对路径
    `policy_id`                         VARCHAR(32)                     NOT NULL,       -- 策略ID
    `status`                            TINYINT         DEFAULT 0       NOT NULL,       -- 处理状态，0 - 未处理，5 - 删除，6 - 误报, 7,8 - 权限文件制作
    `scan_finished_time`                DATETIME        DEFAULT 0       NOT NULL,       -- 扫描完成时间
    `action_id`                         VARCHAR(32)     DEFAULT ''      NOT NULL,       -- 策略ID
    `file_type`                         VARCHAR(8),                                     -- 文件类型
    `file_size`                         VARCHAR(16)     DEFAULT ''      NOT NULL,       -- 文件大小
    `is_recognized`                     INTEGER         DEFAULT 1,                      -- 文件是否识别
    `is_blocked`                        INTEGER         DEFAULT 1,                      -- 文件是否识别
    `detect_result`                     TEXT            DEFAULT ''      NOT NULL,       -- 扫描结果
    `status_reported`                   TINYINT         DEFAULT 0       NOT NULL,       -- 是否需要上报处理，0 - 不需要；1 - 需要
    UNIQUE(scan_file_name,policy_id)
);
