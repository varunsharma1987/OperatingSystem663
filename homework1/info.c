#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

static struct music_info *head;
static struct music_info *curr;

/*the routine function, deals with the loop body of the main program*/
void
routine()
{
  printf("Welcome to MP3 infomation center!\n");
  char input[2];
  while (1) {
    printf("\nPlease indicate the operation, <q> to exit \n (<i>nsert, <r>emove), or <v>iew info: ");
    gets(input);
    //printf("The input was %s\n", input);
    if (strcmp(input, "q") == 0) {
       printf("Are you sure to exit? [Y/N]: ");
       gets(input);
       if (strcmp(input, "y") == 0 || strcmp(input, "Y") == 0)
         break;
       continue;
    }
    else if (strcmp(input, "i") == 0) {
        insert_song();
        continue;
    }
    else if (strcmp(input, "r") == 0) {
        remove_song();
        continue;
    }
    else if (strcmp(input, "v") == 0) {
        view_database();
        continue;
    }
    else printf("\nSorry, your input is invalid. Please try again.");
  }
}

/*initialize the doubly linked list*/
void
list_init (void)
{
  head = NULL;
  curr = NULL;
}     

/*create a new node*/
struct music_info*
new_node (void)
{
  struct node* curr_node = (struct music_info*) malloc (sizeof (struct music_info));
  return curr_node;
}

/*Append the new node into the list*/
void
list_append (struct music_info* newnode)
{
  if (head == NULL) {
    head = newnode;
    newnode -> prev = newnode;
    newnode -> next = newnode;
  }
  else {
    struct music_info* then_head = head;
    struct music_info* then_tail = head -> prev;
    head = newnode;
    then_tail -> next = newnode;
    then_head -> prev = newnode;
    newnode -> prev = then_tail;
    newnode -> next = then_head;
  }
}

/*Delete a node from the doubly linked list*/
void
list_delete_node (struct music_info* to_delete)
{
  free(to_delete->title);
  free(to_delete->artist);
  free(to_delete->album);
  free(to_delete->date);
  if (to_delete -> prev == to_delete) {
    free (to_delete);
    head = NULL;
  }
  else if (head == to_delete){
    struct music_info* new_head = to_delete -> next;
    struct music_info* tail = head -> prev;
    head = new_head;
    tail -> next = new_head;
    new_head -> prev = tail;
    free (to_delete);
  }
  else {
    struct music_info* left = to_delete -> prev;
    struct music_info* right = to_delete -> next;
    left -> next = right;
    right -> prev = left;
    free (to_delete);
  }
}

/*User command: insert a song*/
void
insert_song (void)
{
  char* flag;
  curr = new_node ();
  curr->title = (char*) malloc (sizeof(char[30]));
  curr->artist = (char*) malloc (sizeof(char[50]));
  curr->album = (char*) malloc (sizeof(char[30]));
  curr->date = (char*) malloc (sizeof(char[8]));
  
/*codes on interactive insertion {{{1  */

  printf("[Insert] Please indicate the song's title (max 30 chars)\n    : ");
  flag = gets(curr->title);
  if (!flag)  printf("An error occurred!\n");
  printf("[Insert] Please indicate artist (max 50 chars)\n    : ");
  gets(curr->artist);
  printf("[Insert] Please indicate album (max 30 chars)\n    : "); 
  gets(curr->album);
  printf("[Insert] Please input the song's date (YYYYMMDD) : ");
  gets(curr->date);
  printf("[Insert] Please input the song's running time (in seconds, number only): ");
  scanf("%d", &(curr->run_time));
  printf("[Insert] Insertion completed. The song's name is %s, by %s, in the album %s, written in date %s, and the running time of the song is %d seconds.\n", curr->title, curr->artist, curr->album, curr->date, curr->run_time);

  /*The following two lines of code are used to clear the input buffer.*/
  int c;
  while((c = getchar()) != '\n' && c != EOF);

/*}}}1*/

  list_append(curr);
}

/*User command: remove a song*/
void
remove_song (void)
{
  char artist_del[51], input[2];
  int flag = 0, need_return = 0;;
  printf("[Remove song] ");
  if (!head) {
    printf("Sorry, there's no song in the database.\n");
    return;
  }
  curr = head;
  printf("Please indicate the artist you want to delete: \n    (Type full name of artist): ");
  gets(artist_del);
  while (1)  {
    if (strcmp((curr->artist), artist_del) == 0) {
      flag = 1;
    }
    curr = curr -> next;
    if (curr == head) break;
  }
  
  while (flag) {
    printf("[Remove song] This will delete ALL SONGS of the given ARTIST, is that okay?\n  <Y>es, <N>o: ");
    gets(input);
    if (strcmp(input, "N") == 0 || strcmp(input, "n") == 0) return;
    if (strcmp(input, "Y") == 0 || strcmp(input, "y") == 0) {
      curr = head;
      while (head) {
        if (strcmp ((curr->artist), artist_del) == 0) {
          struct music_info *delnode = curr;
          curr = curr -> next;
          if (curr == head)  need_return = 1;
          printf("[Remove song] Deleting song %s... ", delnode -> title);
          list_delete_node(delnode);
          printf("done.\n");
        }
        else {
          curr = curr -> next;
          if (curr == head)  need_return = 1;
        }
        if (need_return)  return;
      }
    }
  } 

  if (!flag) {
    printf("[Remove song] Sorry, there's no song by the artist %s\n", artist_del);
    return;
  }
}

/*User command: view the song database*/
void
view_database (void)
{
  curr = head;
  printf("\n[View Database]  ");
  while (head) {
    printf("\n---Song Information---\n");
    printf("Title: %s\n", curr->title);
    printf("Artist: %s\n", curr->artist);
    printf("Album: %s\n", curr->album);
    printf("Date: %s\n", curr->date);
    printf("Length: %d sec.\n", curr->run_time);
    curr = curr->next;
    if (curr == head) break;
  }
  if (!head) {
    printf("Sorry, there's no song in the database.\n");
  }
}

/*does some last job before exit, being nice to the OS*/
void
before_exit (void)
{
  while (head) {
    list_delete_node (head);
  }

  printf("\nThanks for using, have a nice day!\n");
}
