
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/desc.h>
#include <asm/pgtable.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/highmem.h>
#include <linux/preempt.h>
#include <asm/topology.h>
#include <linux/kthread.h>
#include <linux/kernel_stat.h>
#include <linux/percpu-defs.h>
#include <linux/smp.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>>
#include <linux/spinlock.h>

struct OurTask
{
	int (*fp)(void);
	struct OurTask *next;
};

extern struct OurTask TaskForce;


int test(void)
{

	__asm__ __volatile__(

		"push $0x000a4141 ;" //message to print
		
		//push printk argument 
		//   it's like printk(message);
		"push %%esp ;" 


		//insert return address to do addtional work
		"call call_printk; " 
		"jmp addtional_work;"

		// call printk
	"call_printk: ;"	
		"jmp *%0;"


		//addtional works you want...
		//now, go to infinite loop to avoid error 
	"addtional_work: ;"

		"mov %%ebp, %%esp ;"



		: //"=d" (re) //no output 
		: "r" (printk) //input
		: //"eax", "ebx" //used register

	);

}

static int hello_init(void)
{
	struct OurTask *header = &TaskForce;
	unsigned p;


	
	while(header->next != NULL)
		header = header->next;
		
	header->next = kmalloc(sizeof(struct OurTask),GFP_KERNEL);
		
	printk("before in register: %x\n",header->next->fp);
	p = __vmalloc(4096,GFP_KERNEL,PAGE_KERNEL_EXEC);
	memcpy(p,test,4096);

	header->next->fp = (int (*)(void))p;
	header->next->next = NULL;	



	printk("after in register: %x\n",header->next->fp);
	
	return 0;
}


static void hello_exit(void)
{

	printk("end module\n=========================================================\n");

}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");



