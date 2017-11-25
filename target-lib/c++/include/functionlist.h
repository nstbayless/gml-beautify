// real-valued functions
VO lerp(C, V& a, V& b, V& amt);
VO clamp(C, V& val, V& min, V& max);

/// list data structure
VO ds_list_create(C);
void ds_list_destroy(C, V& id);
void ds_list_add(C, V& id, V& value);
void ds_list_insert(C, V& id, V& pos, V& value);
VO ds_list_read(C, V& id, V& index);