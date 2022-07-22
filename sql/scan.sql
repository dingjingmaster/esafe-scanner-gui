
-- 扫描任务表，用于记录任务最新信息
CREATE TABLE scan_task (
    `task_id`                           VARCHAR(255)    DEFAULT 0       NOT NULL,       -- 任务 ID
    `scan_interval`                     TINYINT         DEFAULT 4       NOT NULL,       -- 变动扫描定时时长(默认 4 小时)
    `scan_task_filter_name`             TEXT                            NOT NULL,       -- 扫描任务过滤策略，以分号分割
    `scan_task_dir`                     TEXT                            NOT NULL,       -- 扫描文件夹，以分号分割
    `scan_task_dir_filterout`           TEXT                            NOT NULL,       -- 扫描例外文件夹，以分号分割
    `scan_task_dir_filterout_fileext`   TEXT                            NOT NULL,       -- 扫描任务过滤的文件类型，以分号分割

    `scan_result_reuse`                 TINYINT         DEFAULT 0       NOT NULL,       -- 扫描结果复用：0 - 开启，1 - 关闭
    `scan_task_file_monitor`            TINYINT         DEFAULT 0       NOT NULL,       -- 变动文件监控：0 - 开启，1 - 关闭
    `scan_task_self_check`              TINYINT         DEFAULT 0       NOT NULL,       -- 自查任务：0 - 开启，1 - 关闭
    PRIMARY KEY(task_id)
);


-- 扫描任务轮次表，保存任务轮次数据
CREATE TABLE scan_task_freq (
    `task_id`                           VARCHAR(255)                    NOT NULL,       -- 任务ID
    `scan_id`                           INT             DEFAULT 0       NOT NULL,       -- 任务扫描轮次
    `task_status`                       TINYINT         DEFAULT 0       NOT NULL,       -- 任务状态：0 - 未知，1 - 进行中，2 - 暂停，3 - 停止
    `task_start_time`                   DATETIME        DEFAULT 0       NOT NULL,       -- 开始时间 
    `task_stop_time`                    DATETIME        DEFAULT 0       NOT NULL,       -- 结束时间 
    `task_file_count`                   INT4            DEFAULT 0       NOT NULL,       -- 终端总文件数
    `task_scan_file_count`              INT4            DEFAULT 0       NOT NULL,       -- 需要扫描的文件总数
    `task_scan_finished_file_count`     INT4            DEFAULT 0       NOT NULL,       -- 扫描完成的文件数
    PRIMARY KEY(task_id,scan_id)
);

-- 中间数据
-- 扫描过程中 临时数据表(根据任务id+任务轮次 自建)
CREATE TABLE scan_task_detail_taskid_1 (
    `detail_file_name_md5`              VARCHAR(255)                    NOT NULL,       -- 扫描的文件名
    `detail_file_name`                  TEXT                            NOT NULL,       -- 扫描文件路径
    `detail_file_status`                TINYINT         DEFAULT 0       NOT NULL,       -- 文件是否扫描：0 - 未扫描，1 - 扫描完成
    PRIMARY KEY(detail_file_name_md5)
);


-- 扫描结果，仅保存被扫描任务命中的文件
CREATE TABLE scan_result (
    `scan_file_name_md5`                VARCHAR(255)                    NOT NULL,       -- 根据文件绝对路径生成的 MD5
    `scan_file_name`                    TEXT                            NOT NULL,       -- 文件绝对路径
    `task_id`                           VARCHAR(255)    DEFAULT 0       NOT NULL,       -- 任务ID           冗余，保存的是最后一次命中的任务 ID
    `scan_id`                           INT             DEFAULT 0       NOT NULL,       -- 任务扫描轮次     冗余，保存的是最后一次命中的任务 ID
    `filter_name`                       VARCHAR(255)                    NOT NULL,       -- 扫描命中策略
    `status`                            TINYINT         DEFAULT 0       NOT NULL,       -- 处理状态，0 - 未处理，1 - 删除，2 - 误报
    `file_create_time`                  DATETIME        DEFAULT 0       NOT NULL,       -- 文件创建时间
    `file_modify_time`                  DATETIME        DEFAULT 0       NOT NULL,       -- 文件修改时间
    PRIMARY KEY(scan_file_name_md5,filter_name)
);
