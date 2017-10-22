#include "dc_malloc.h"

m_manager memman_g;

void debug_print( void )
{
	pm_header thdr = NULL;
	pm_footer tftr = NULL;

	if ( memman_g.free_list == NULL ) {
		printf("Empty\n");
	}

	thdr = (pm_header)memman_g.free_list;

	while ( thdr != NULL ) {
//		tftr = BLOCK_FOOTER(thdr);

		tftr = ((pm_footer)(  ((char *)thdr) + (( ( ((pm_header)thdr)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));

		printf("Addr: %p Size: %lx Next: %p Prev: %p\n", thdr, thdr->size, tftr->pNext, tftr->pPrev);

		thdr = (pm_header)tftr->pNext;
	}

	return;
}

void *add_free_block( unsigned int size)
{
	void *new_block = NULL;
	pm_header thdr;
	pm_footer tftr;

	/// Adjust size+sizeof(malloc header) up to the nearest page
	size += sizeof( m_header );

	if ( size % PAGE_SIZE ) {
		size /= PAGE_SIZE;
		size += 1;
		size *= PAGE_SIZE;
	}

//	new_block = mmap( NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	new_block = malloc(size);

	if ( new_block == NULL ) {
		exit(-1);
	}
	mprotect(new_block, size, MPROTECT_READWRITE | MPROTECT_READEXECUTE);

	thdr = (pm_header)new_block;

	thdr->size = size - sizeof(m_header);

//	tftr = BLOCK_FOOTER( new_block );
	tftr = ((pm_footer)(  ((char *)new_block) + (( ( ((pm_header)new_block)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));

	tftr->pNext = NULL;
	tftr->pPrev = memman_g.free_list;

	memman_g.free_list = new_block;

	return new_block;
}


void *dc_malloc( unsigned int size )
{
	void *freeWalker = NULL;
	void *final_alloc = NULL;
	void *new_block = NULL;
	unsigned int size_left = 0;

	pm_header thdr;
	pm_footer tftr;
	pm_header header_new_block;
	pm_footer footer_new_block;

	/// Each block needs to be a least the size of the footer structure
	if ( size < sizeof( m_footer ) ) {
		size = sizeof(m_footer);
	}

	/// Align to 8 bytes
	if ( size % 8 ) {
		size = ( size >> 3 ) + 1;
		size <<= 3;
	}

	freeWalker = memman_g.free_list;

	while ( 1 ) {
		if ( freeWalker == NULL ) {
			freeWalker = add_free_block( size );
		}

		thdr = (pm_header)freeWalker;
//		tftr = BLOCK_FOOTER( freeWalker );
		tftr = ((pm_footer)(  ((char *)freeWalker) + (( ( ((pm_header)freeWalker)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));

		/// Check if the current block is large enough to fulfill the request
		/// If so then downsize as needed
		if ( ( thdr->size & ~3) >= size ) {
			final_alloc = freeWalker + sizeof(pm_header);

			size_left = (thdr->size& ~3) - size;

			/// Set the in use flag
			thdr->size |= 1;

			// If there is room then create a new block
			if ( size_left > sizeof(m_header) + sizeof( m_footer) ) {

				// Fix up the size
				thdr->size = size;
				thdr->size |= 1;

				// Set the flag indicating that there is a block following the previous one
				thdr->size |= 2;

				new_block = final_alloc + size;

				header_new_block = (pm_header)new_block;
				header_new_block->size = size_left - sizeof(m_header);
				footer_new_block = tftr;

				/// If this was the head of the list then fix it up
				if ( freeWalker == memman_g.free_list ) {

					memman_g.free_list = new_block;


					if ( tftr->pNext != NULL ) {
//						tftr = BLOCK_FOOTER( (void*)(tftr->pNext) );
						tftr = ((pm_footer)(  ((char *)(tftr->pNext)) + (( ( ((pm_header)(tftr->pNext))->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));
						tftr->pPrev = (pm_header)new_block;
					}
				} else {

					// Fix up next and prev
					if ( tftr->pPrev != NULL ) {
						freeWalker = tftr->pPrev;
//						tftr = BLOCK_FOOTER( freeWalker );
						tftr = ((pm_footer)(  ((char *)freeWalker) + (( ( ((pm_header)freeWalker)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));
						tftr->pNext = (pm_header)new_block;

						tftr = footer_new_block;
					}

					if ( tftr->pNext != NULL ) {
						freeWalker = tftr->pNext;
//						tftr = BLOCK_FOOTER( freeWalker);
						tftr = ((pm_footer)(  ((char *)freeWalker) + (( ( ((pm_header)freeWalker)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));
						tftr->pPrev = (pm_header)new_block;
					}

				}

			} else {

				/// Just unlink and return this one
				if ( freeWalker == memman_g.free_list ) {
					memman_g.free_list = tftr->pNext;

					if ( memman_g.free_list ) {
//						tftr = BLOCK_FOOTER( (void*)(memman_g.free_list) );
						tftr = ((pm_footer)(  ((char *)(memman_g.free_list) + (( ( ((pm_header)(memman_g.free_list)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))))));
						tftr->pPrev = NULL;
					}

				} else {
					// Fix up next and prev
					if ( tftr->pPrev != NULL ) {
						freeWalker = tftr->pPrev;
//						pm_footer unlink_ftr = BLOCK_FOOTER( freeWalker );
						pm_footer unlink_ftr = ((pm_footer)(  ((char *)freeWalker) + (( ( ((pm_header)freeWalker)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));
						unlink_ftr->pNext = tftr->pNext;
					}

					if ( tftr->pNext != NULL ) {
						freeWalker = tftr->pNext;

//						pm_footer unlink_ftr = BLOCK_FOOTER( freeWalker);
						pm_footer unlink_ftr = ((pm_footer)(  ((char *)freeWalker) + (( ( ((pm_header)freeWalker)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));
						unlink_ftr->pPrev = tftr->pPrev;
					}
				}
			}

			/// Fix ups are done. Return the allocation
			return final_alloc;
		}

		freeWalker = (void*)tftr->pNext;

	}
}

void dc_free( void *block )
{
	pm_header thdr;
	pm_header nhdr;
	pm_footer tftr;
	pm_footer nftr;
	unsigned long tsize = 0;

	if ( block == NULL ) {
		return;
	}

	thdr = block-sizeof(m_header);

	// Check the in use bit
	if ( !(thdr->size & 1) ) {
		return;
	}


	/// If there is a block following this one then check if it is in use. If not then coalesce
	if ( thdr->size & 2 ) {
		tsize = thdr->size >> 3;
		tsize <<= 3;

		nhdr = block + tsize;


		if ( !(nhdr->size & 1 ) ) {

			/// Update the sizes
			thdr->size += ( nhdr->size & ~3) + sizeof( m_header);

			/// If the next block does not have a block after it then remove the flag
			if ( !(nhdr->size & 2 ) ) {
				thdr->size ^= 2;
			}

			/// If the next block was the previous head then update that
			/// Otherwise the prev and next pointers need to be updated
			if ( memman_g.free_list == (void*)nhdr) {
				memman_g.free_list = (void*)thdr;	
			}

			// Update the next->prev if necessary
			tftr = BLOCK_FOOTER(thdr);
			tftr = ((pm_footer)(  ((char *)thdr) + (( ( ((pm_header)thdr)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));

			if ( tftr->pNext ) {
//				nftr = BLOCK_FOOTER(tftr->pNext);
				nftr = ((pm_footer)(  ((char *)(tftr->pNext)) + (( ( ((pm_header)(tftr->pNext))->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));

				nftr->pPrev = thdr;
			}

			if ( tftr->pPrev ) {
//				nftr = BLOCK_FOOTER(tftr->pPrev);
				nftr = ((pm_footer)(  ((char *)(tftr->pNext)) + (( ( ((pm_header)(tftr->pNext))->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));

				nftr->pNext = thdr;
			}
			return;
		}
	}

	// If we reach here then it did not coalesce. Just stick it at the front
//	tftr = BLOCK_FOOTER( (void*)thdr );
	tftr = ((pm_footer)(  ((char *)thdr) + (( ( ((pm_header)thdr)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));

	/// Remove the in use flag
	thdr->size ^= 1;

	tftr->pNext = memman_g.free_list;
	tftr->pPrev = NULL;

	/// Base case
	if ( memman_g.free_list != NULL ) {
//		nftr = BLOCK_FOOTER( memman_g.free_list );
		nftr = ((pm_footer)(  ((char *)memman_g.free_list) + (( ( ((pm_header)memman_g.free_list)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))));

		nftr->pPrev = thdr;
	}

	memman_g.free_list = thdr;

	return;
}
