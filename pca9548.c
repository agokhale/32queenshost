#include "pca9548.h"
//http://www.mouser.com/ds/2/302/PCA9548A-185768.pdf

void pca9548_set_page ( brctx_t * brctx,  u8 i2_addr,  u8 inpage) 
{
i2msg *cmd ;
i2msg *response;
assert ( inpage < 8); 
cmd = i2alloc ( 1 );
cmd->addr = i2_addr;
cmd->cmd = ki2cmd_write;
cmd->data[0] = 1 << inpage ; // the register is the only thing that 9548 responds to
whisper ('9', "setting page to %x ->  0x%x", inpage, cmd->data[0]);  
response = i2msg_do ( brctx,  cmd) ;
}

void pca9548_get_page ( brctx_t * brctx, u8 i2_addr) 
//broken!!
{
i2msg *cmd ;
i2msg *response;
cmd = i2alloc ( 0 );
cmd->addr = i2_addr;
cmd->cmd = ki2cmd_read;
cmd->len = 1;
response = i2msg_do (  brctx, cmd) ;
i2dump( response); 


}
