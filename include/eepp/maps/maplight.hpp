#ifndef EE_MAPS_CLIGHT_H
#define EE_MAPS_CLIGHT_H

#include <eepp/maps/base.hpp>

namespace EE { namespace Maps {

/** @enum LIGHT_TYPE Define the light spot type */
enum LIGHT_TYPE {
	LIGHT_NORMAL	= 0,
	LIGHT_ISOMETRIC = 1
};

class EE_API MapLight {
	public:
		MapLight();

		MapLight( const Float& Radius, const Float& x, const Float& y, const RGB& Color = RGB(255,255,255), LIGHT_TYPE Type = LIGHT_NORMAL );

		virtual ~MapLight();

		void create( const Float& Radius, const Float& x, const Float& y, const RGB& Color = RGB(255,255,255), LIGHT_TYPE Type = LIGHT_NORMAL );

		virtual RGB processVertex( const Float& PointX, const Float& PointY, const RGB& VertexColor, const RGB& BaseColor );

		virtual Color processVertex( const Float& PointX, const Float& PointY, const Color& VertexColor, const Color& BaseColor );

		RGB processVertex( const Vector2f& Pos, const RGB& VertexColor, const RGB& BaseColor );

		Color processVertex( const Vector2f& Pos, const Color& VertexColor, const Color& BaseColor );

		void move( const Float& addtox, const Float& addtoy );

		void updatePos( const Float& x, const Float& y );

		void updatePos( const Vector2f& newPos );

		Rectf getAABB() const;

		const Float& getRadius() const;

		void setRadius( const Float& radio );

		const bool& isActive() const;

		void setActive( const bool& active );

		void setColor( const RGB& color );

		const RGB& getColor() const;

		void setType( const LIGHT_TYPE& type );

		const LIGHT_TYPE& getType() const;

		const Vector2f& getPosition() const;

		void setPosition( const Vector2f& newPos );
	protected:
		Float		mRadius;
		Vector2f	mPos;
		RGB		mColor;
		LIGHT_TYPE	mType;
		Rectf		mAABB;
		bool		mActive;

		void updateAABB();
};

}}

#endif
