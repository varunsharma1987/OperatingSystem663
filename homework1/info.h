#ifndef H_INFO
#define H_INFO

struct music_info {
  char* artist;
  char* album;
  char* title;
  char* date;
  int run_time;
  struct music_info *prev;
  struct music_info *next;
};

/*the routine function, deals with the loop body of the main program*/
void
routine (void);

/*initialize the doubly linked list*/
void
list_init (void);

/*Create a new node*/
struct music_info*
new_node (void);

/*Append the new node into the list*/
void
list_append (struct music_info*);

/*Delete a node from the doubly linked list*/
void
list_delete_node (struct music_info*);

/*User command: insert a song*/
void
insert_song (void);

/*User command: remove a song*/
void
remove_song (void);

/*User command: view the song database*/
void
view_database (void);

/*does some last job before exit, being nice to the OS*/
void
before_exit (void);
#endif
