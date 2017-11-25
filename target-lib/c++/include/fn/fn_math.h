///// trigonometric functions /////

VO arccos(C, V&);
VO arcsin(C, V&);
VO arctan(C, V&);
VO arctan2(C, V& a);
VO sin(C, V&);
VO tan(C, V&);
VO cos(C, V&);
VO darccos(C, V&);
VO darcsin(C, V&);
VO darctan(C, V&);
VO darctan2(C, V& a);
VO dsin(C, V&);
VO dtan(C, V&);
VO dcos(C, V&);
VO degtorad(C, V&);
VO radtodeg(C, V&);
VO lengthdir_x(C, V& len, V& dir);
VO lengthdir_y(C, V& len, V& dir);


///// rounding functions /////

VO round(C, V&);
VO floor(C, V&);
VO frac(C, V&);
VO abs(C, V&);
VO sign(C, V&);
VO ceil(C, V&);
VO max(C, byte n, V&*);
VO mean(C, byte n, V&*);
VO median(C, byte n, V&*);
VO min(C, byte n, V&*);
VO lerp(C, V& a, V& b, V& amt);
VO clamp(C, V& val, V& min, V& max);


///// misc functions /////

VO exp(C, V&);
VO ln(C, V&);
VO power(C, V& base, V& exponent);
VO sqr(C, V&);
VO sqrt(C, V&);
VO log2(C, V&);
VO log10(C, V&);
VO logn(C, V&, V& n);
VO int64(C, V&, V& n);


///// vector functions /////

VO point_direction(C, V& x1, V& y1, V& x2, V& y2);
VO point_distance(C, V& x1, V& y1, V& x2, V& y2);
VO point_distance_3d(C, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2);
VO dot_product(C, V& x1, V& y1, V& x2, V& y2);
VO dot_product_3d(C, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2);
VO dot_product_normalised(C, V& x1, V& y1, V& x2, V& y2);
VO dot_product_normalised_3d(C, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2);
VO angle_difference(C, V& a1, V& a2);

///// list data structure /////

//! returns list id, guaranteed to be positive
VO ds_list_create(C);
void ds_list_destroy(C, V& id);
void ds_list_add(C, V& id, V& value);
void ds_list_insert(C, V& id, V& pos, V& value);
VO ds_list_read(C, V& id, V& index);