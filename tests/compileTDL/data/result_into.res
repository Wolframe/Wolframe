TRANSACTION testcall
BEGIN
Code:
[0] OUTPUT_OPEN TAG result
[1] RESULT_SET_INIT
[2] DBSTM_START STM (SELECT run( $1 , $2 ))
[3] DBSTM_BIND_PATH PATH /aa
[4] DBSTM_BIND_PATH PATH /bb
[5] DBSTM_EXEC
[6] OUTPUT_OPEN_ARRAY TAG item
[7] OPEN_ITER_LAST_RESULT
[8] NOT_IF_COND GOTO @14
[9] OUTPUT_OPEN_ELEM
[10] OUTPUT_ITR_COLUMN
[11] OUTPUT_CLOSE_ELEM
[12] NEXT
[13] IF_COND GOTO @9
[14] OUTPUT_CLOSE_ARRAY
[15] RESULT_SET_INIT
[16] OPEN_ITER_LAST_RESULT
[17] NOT_IF_COND GOTO @24
[18] DBSTM_START STM (SELECT exec ( $1,$2))
[19] DBSTM_BIND_ITR_IDX COLIDX 1
[20] DBSTM_BIND_ITR_IDX COLIDX 2
[21] DBSTM_EXEC
[22] NEXT
[23] IF_COND GOTO @18
[24] OUTPUT_CLOSE
[25] RETURN
END

