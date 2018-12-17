#define PML4_FLAGS 0x1F
#define PDPE_FLAGS 0x17
#define PDE_FLAGS 0x87 // (0x9F for uncacheable as default, 0x87 for cacheable as default)


#define MEM_MAX 512

.data
value:
.long 2

.code32

.global setup_paging
.global enable_paging



.text
setup_paging:

// Just insert one entry in PML4
fill_pml4:
	mov $PML4_FLAGS, %ecx
	// Add PDPE address to PML4 entry
	or $global_pdpe, %ecx
	// Write first PML4 entry
	mov %ecx, global_pml4

fill_pdpe:
	// Loop termination (512 = 512 GB MAXIMUM)
	mov $MEM_MAX, %ebx
	// Loop index
	mov $0, %esi
	// Load the PDE base address
	mov $global_pde,%eax
	// Add the flags to the first entry
	or $PDPE_FLAGS, %eax
fill_pdpe_loop:
	// Check termination condition
	cmp %esi, %ebx
	// Abort if reached
	je fill_pdpe_finish
	// Write lower half of entry (32 bits)
	movl %eax, global_pdpe(,%esi,8)
	// 4 bytes offset (disp(base, index, scale) = [base + index * scale + disp]
	mov $4, %edi
	// Write upper half of entry (32 bits)
	// Unused -> 0
	movl $0,  global_pdpe(%edi,%esi,8)
	// Increase entry index
	add $1, %esi
	// Increase PDE base pointer by 0x1000 (4096(512 entries * 8 bytes) PDE size)
	add $0x1000, %eax
	// Loop
	jmp fill_pdpe_loop
fill_pdpe_finish:

fill_pde:
	// Loop termination (512 entries)
	mov $512 * MEM_MAX, %ebx
	// Loop index
	mov $0, %esi
	// Load the base physical address (0x0)
	mov $0,%eax
	// Add the flags to the first entry
	or $PDE_FLAGS, %eax
fill_pde_loop:
	// Check termination condition
	cmp %esi, %ebx
	// Abort if reached
	je fill_pde_finish
	// Write lower half of entry (32 bits)
	movl %eax, global_pde(,%esi,8)
	// 4 bytes offset (disp(base, index, scale) = [base + index * scale + disp]
	mov $4, %edi
	// Write upper half of entry (32 bits)
	// Unused -> 0
	movl $0,  global_pde(%edi,%esi,8)
	// Increase entry index
	add $1, %esi
	// Increase PDE base pointer by 0x200000 (2 megabytes page size)
	add $0x200000, %eax
	// Loop
	jmp fill_pde_loop

fill_pde_finish:
enable_paging:
	// Writeback cache contents
	WBINVD
	// Set base pointer
    mov     $global_pml4, %eax
	mov     %eax, %cr3

	ret

