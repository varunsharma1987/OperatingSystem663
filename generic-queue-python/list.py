#!/usr/bin/python3

######## Function Decls ########

def list_append(target_list, to_append):
    """ Append a new item to the existed list. """
    target_list.append(to_append)
    return None

def list_remove(target_list, remove_id):
    """ Remove the specified item from list. """
    target_list.pop(remove_id)
    return None

######## Program entry ########

testlist = list()
print("testlist initialized as: ", testlist)
temp_tuple = (3, 4)
list_append(testlist, temp_tuple)
print("after first append: ", testlist)
temp_tuple = (1, 2)
list_append(testlist, temp_tuple)
print("after second append: ", testlist)

for i in range(0, len(testlist)):
    if testlist[i] == (1, 2):
        break
list_remove(testlist, i)

print("after remove: ", testlist)
print("Done!\n")
