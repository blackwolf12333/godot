#include "light_occluder_2d.h"


void OccluderPolygon2D::set_polygon(const DVector<Vector2>& p_polygon) {

	polygon=p_polygon;
	VS::get_singleton()->canvas_occluder_polygon_set_shape(occ_polygon,p_polygon,closed);
	emit_changed();
}

DVector<Vector2> OccluderPolygon2D::get_polygon() const{

	return polygon;
}

void OccluderPolygon2D::set_closed(bool p_closed) {

	if (closed==p_closed)
		return;
	closed=p_closed;
	VS::get_singleton()->canvas_occluder_polygon_set_shape(occ_polygon,polygon,closed);
	emit_changed();
}

bool OccluderPolygon2D::is_closed() const{

	return closed;
}

void OccluderPolygon2D::set_cull_mode(CullMode p_mode){

	cull=p_mode;
	VS::get_singleton()->canvas_occluder_polygon_set_cull_mode(occ_polygon,VS::CanvasOccluderPolygonCullMode(p_mode));
}

OccluderPolygon2D::CullMode OccluderPolygon2D::get_cull_mode() const{

	return cull;
}


RID OccluderPolygon2D::get_rid() const {

	return occ_polygon;
}

void OccluderPolygon2D::_bind_methods() {


	ObjectTypeDB::bind_method(_MD("set_closed","closed"),&OccluderPolygon2D::set_closed);
	ObjectTypeDB::bind_method(_MD("is_closed"),&OccluderPolygon2D::is_closed);

	ObjectTypeDB::bind_method(_MD("set_cull_mode","cull_mode"),&OccluderPolygon2D::set_cull_mode);
	ObjectTypeDB::bind_method(_MD("get_cull_mode"),&OccluderPolygon2D::get_cull_mode);

	ObjectTypeDB::bind_method(_MD("set_polygon","polygon"),&OccluderPolygon2D::set_polygon);
	ObjectTypeDB::bind_method(_MD("get_polygon"),&OccluderPolygon2D::get_polygon);

	ADD_PROPERTY( PropertyInfo(Variant::VECTOR2_ARRAY,"polygon"),_SCS("set_polygon"),_SCS("get_polygon"));
	ADD_PROPERTY( PropertyInfo(Variant::BOOL,"closed"),_SCS("set_closed"),_SCS("is_closed"));
	ADD_PROPERTY( PropertyInfo(Variant::INT,"cull_mode",PROPERTY_HINT_ENUM,"Disabled,ClockWise,CounterClockWise"),_SCS("set_cull_mode"),_SCS("get_cull_mode"));

	BIND_CONSTANT(CULL_DISABLED);
	BIND_CONSTANT(CULL_CLOCKWISE);
	BIND_CONSTANT(CULL_COUNTER_CLOCKWISE);
}


OccluderPolygon2D::OccluderPolygon2D() {

	occ_polygon=VS::get_singleton()->canvas_occluder_polygon_create();
	closed=true;
	cull=CULL_DISABLED;
}

OccluderPolygon2D::~OccluderPolygon2D() {

	VS::get_singleton()->free(occ_polygon);
}

#ifdef DEBUG_ENABLED
void LightOccluder2D::_poly_changed() {

	update();
}
#endif


void LightOccluder2D::_notification(int p_what) {

	if (p_what==NOTIFICATION_ENTER_CANVAS) {

		VS::get_singleton()->canvas_light_occluder_attach_to_canvas(occluder,get_canvas());
		VS::get_singleton()->canvas_light_occluder_set_transform(occluder,get_global_transform());

	}
	if (p_what==NOTIFICATION_TRANSFORM_CHANGED) {

		VS::get_singleton()->canvas_light_occluder_set_transform(occluder,get_global_transform());
	}

	if (p_what==NOTIFICATION_DRAW) {

		if (get_tree()->is_editor_hint()) {

			if (occluder_polygon.is_valid()) {

				DVector<Vector2> poly = occluder_polygon->get_polygon();

				if (poly.size()) {
					if (occluder_polygon->is_closed()) {
						Vector<Color> color;
						color.push_back(Color(0,0,0,0.6));
						draw_polygon(Variant(poly),color);
					} else {

						int ps=poly.size();
						DVector<Vector2>::Read r = poly.read();
						for(int i=0;i<ps-1;i++) {

							draw_line(r[i],r[i+1],Color(0,0,0,0.6),3);
						}
					}
				}
			}
		}
	}


	if (p_what==NOTIFICATION_EXIT_CANVAS) {

		VS::get_singleton()->canvas_light_occluder_attach_to_canvas(occluder,RID());
	}


}

void LightOccluder2D::set_occluder_polygon(const Ref<OccluderPolygon2D>& p_polygon) {

#ifdef DEBUG_ENABLED
	if (occluder_polygon.is_valid())
		occluder_polygon->disconnect("changed",this,"_poly_changed");
#endif
	occluder_polygon=p_polygon;

	if (occluder_polygon.is_valid())
		VS::get_singleton()->canvas_light_occluder_set_polygon(occluder,occluder_polygon->get_rid());
	else
		VS::get_singleton()->canvas_light_occluder_set_polygon(occluder,RID());

#ifdef DEBUG_ENABLED
	if (occluder_polygon.is_valid())
		occluder_polygon->connect("changed",this,"_poly_changed");
	update();
#endif

}

Ref<OccluderPolygon2D> LightOccluder2D::get_occluder_polygon() const {

	return occluder_polygon;
}

void LightOccluder2D::set_occluder_light_mask(int p_mask) {

	mask=p_mask;
	VS::get_singleton()->canvas_light_occluder_set_light_mask(occluder,mask);
}

int LightOccluder2D::get_occluder_light_mask() const{

	return mask;
}

void LightOccluder2D::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("set_occluder_polygon","polygon:OccluderPolygon2D"),&LightOccluder2D::set_occluder_polygon);
	ObjectTypeDB::bind_method(_MD("get_occluder_polygon:OccluderPolygon2D"),&LightOccluder2D::get_occluder_polygon);

	ObjectTypeDB::bind_method(_MD("set_occluder_light_mask","mask"),&LightOccluder2D::set_occluder_light_mask);
	ObjectTypeDB::bind_method(_MD("get_occluder_light_mask"),&LightOccluder2D::get_occluder_light_mask);

#ifdef DEBUG_ENABLED
	ObjectTypeDB::bind_method("_poly_changed",&LightOccluder2D::_poly_changed);
#endif

	ADD_PROPERTY( PropertyInfo(Variant::OBJECT,"occluder",PROPERTY_HINT_RESOURCE_TYPE,"OccluderPolygon2D"),_SCS("set_occluder_polygon"),_SCS("get_occluder_polygon"));
	ADD_PROPERTY( PropertyInfo(Variant::INT,"light_mask",PROPERTY_HINT_ALL_FLAGS),_SCS("set_occluder_light_mask"),_SCS("get_occluder_light_mask"));
}

LightOccluder2D::LightOccluder2D() {

	occluder=VS::get_singleton()->canvas_light_occluder_create();
	mask=1;
}

LightOccluder2D::~LightOccluder2D() {

	VS::get_singleton()->free(occluder);
}

