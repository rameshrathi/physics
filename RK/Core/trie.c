//

#include "lib.h"

typedef struct TrieNode {
	
	char key;
	
	int ch_count;
	
	struct TrieNode ** childrens;

}
TrieNode;


void store_word( TrieNode *root, const char *word)
{
	size_t len = str_len(word);
	TrieNode * curr = root;

	for (int idx = 0; idx < len; idx++)
	{
		char  ch = word[idx];

		while (curr)
		{
			if (curr->key == ch)
			{
				break;
			}
		}
	}
}

int is_valid(const char *key);

