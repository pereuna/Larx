.code16
.text
.global _start

_start:
/* asetetaan datasegmentti yms. kohalleen */

       	movw $0x07C0, %ax
       	movw %ax, %ds                    
       	movw %ax, %es                   
       	movw $0x400, %sp

/* kirjoitetaan viesti */

	call    write_msg 

/* ladataan kernel osoitteseen 0x2000 */

       	call bios_reset_drive

	movw $0x200, %ax
        movw %ax, %es
        movw $0, %bx   
	
	movw $21, %ax                   # read (blocks of kernel) sector
        movw $1,  %cx                  # starting at sector 2
        
	pusha
	call bios_read_sectors
	popa
	call kill_floppy

/* vapautetaan a20 linja. Muuten osoiteväylä a20 ei toimi pc/ibm yms. */

	movw    $0x2401, %ax
        pushfl             
        int     $0x15
        popfl
	
/* setup global decriptor table */
        
	lgdt    gdtptr   
        
/* muistetaan suojautua */

	cli
	movw $1, %ax		
        lmsw    %ax

/* kernelin datasegmentti 0x10 on alustettavam */	
	
	movw	$0x10, %ax
	movw	%ax, %ds

/* jump to kernel */

	ljmp $0x8, $0x2000

/* siihen loppui pääohjelma */
/* funktiot */

bios_reset_drive:
        pusha
        movb $0, %ah         
        int  $0x13	
	popa
        ret

kill_floppy:

	pusha
	movw	$0x3f2, %dx
	movw	$0x0, %ax
	outw	%ax, %dx
	popa
	ret

bios_read_sectors:
/*
input : es:bx = address of destination
         al = sector count
         cl = sector start number
	 dl = disk 0=floppy
*/			
			
                        movw     %ax, lue_lkm
                        movw     %cx, loog_sect
lue_sect:
                        movw    loog_sect, %ax         #lasketaan pää
                        divb    sectorspertrack        #sektoreita uralla=12h,
                                                       #tulos=al,
                                                       #jakojäännös=ah
                        movb     $0, %ah
                        divb     numberofheads         #levypinnat lkm->ah=head
                        movb     %ah,%dh	       #dh = head number(0x13)

                        movw     loog_sect, %ax        #sektorin laskenta
                        divb     sectorspertrack       #loog_sect % 18
                        incb     %ah                   #ah=jakojäännös + 1
                        movb     %ah, %cl	       #cl=sect number(0x13)

                        movw     loog_sect, %ax    	#sylinterin lasketa
                        divb     sect_heads      	#sektorit * päät
                        movb     %al, %ch		#ch=cyl number(0x13)
							#cl 2 msb=msb for cyl
			
			movb	 $0, %dl	        #dl=0 ,floppy
			movb     $0x02, %ah		#(0x13) read disk
                        movb     $1, %al		# number of sectors read
                        int      $0x13

                        movw     lue_lkm, %cx
                        decw     lue_lkm
                        incw     loog_sect
                        addw     $0x200, %bx		#0x200=sizw of sect
                        loop     lue_sect

                        movw     $0, lue_lkm
                        movw     $0, loog_sect
        ret

write_msg:
	pusha
	movb    $0x03, %ah        # read cursor pos
	xor     %bh,%bh
	int     $0x10

	movw     $18, %cx
	movw     $0x0007, %bx      # page 0, attribute 7 (normal)
	movw     $msg, %bp
	movw     $0x1301, %ax      # write string, move cursor
	int      $0x10
	popa
	ret



/* variaabelit */
			
sect_heads:       .byte 0x24	# numberofheads * sectorspertrack
loog_sect:        .word 0x0000
lue_lkm:          .word 0x0000 
sectorspertrack:  .byte 0x12
numberofheads:    .byte 0x02

# global descriptor table
 
#  |base[31:24]|G|D/B|0|AVL|seg_limit|P|DPL|S|TY(B)E|Base[23:0]|seg_limit|
#  |     8     |1| 1 |1| 1 |    4    |1| 2 |1|  4   |  8  | 16 |   16    |
  
#  	AVL	= Available for system software
#	B	= Bysy flag
#	Base  	= Segment base adress
#	D/B	= data/code=1(32bit) tss=0
#	DPL	= Priviledge level. 0=high 2=low
#	G	= Granularity
#	LIMIT	= Segment limit. must >= 0x67 (if tss)
#	P	= Segment present
#	S	= Descriptor type 0=tss 0=system 1=code/data
#	TY(B)E  = Segment type. TSS = 0x9(|2=bysy)
#				code= 0x8 (execute only)
/*				      0xa (execute/read)*/
#				data  0x2 (read/write)

gdt:    .quad 0x0000000000000000        /* NULL descriptor */
        .quad 0x00c09a00000007ff        /* 8Mb */
	.quad 0x00c09200000007ff        /* 8Mb */
		

gdtptr:
	.word   6*8-1         /* gdt limit: 6 GDT entries */
	.long	0x7c00+gdt    /* gdt base = where is gdt at the moment */

msg:
	.byte 13, 10
	.ascii "Loading Larx ..."
			
/* boot signatude */

.org 510
.word 0xAA55 
