#ifndef EE_GRAPHICSCSUBTEXTURE_H
#define EE_GRAPHICSCSUBTEXTURE_H

#include <eepp/graphics/base.hpp>
#include <eepp/graphics/texture.hpp>
#include <eepp/math/originpoint.hpp>
#include <eepp/graphics/drawableresource.hpp>

namespace EE { namespace Graphics {

/** @brief A SubTexture is a part of a texture that represent an sprite.*/
class EE_API SubTexture : public DrawableResource {
	public:
		/** Creates an empty SubTexture */
		SubTexture();

		/** Creates a SubTexture from a Texture. It will use the full Texture as a SubTexture.
		*	@param TexId The texture id
		*	@param Name The texture name ( if any )
		*/
		SubTexture( const Uint32& TexId, const std::string& name = "" );

		/** Creates a SubTexture of the indicated part of the texture.
		*	@param TexId The texture id
		*	@param SrcRect The texture part that will be used as the SubTexture.
		*	@param Name The texture name ( if any )
		*/
		SubTexture( const Uint32& TexId, const Rect& srcRect, const std::string& name = "" );

		/** Creates a SubTexture of the indicated part of the texture.
		*	@param TexId The texture id
		*	@param SrcRect The texture part that will be used as the SubTexture.
		*	@param DestSize The destination size that the SubTexture will have when rendered.
		*	@param Name The texture name ( if any )
		*/
		SubTexture( const Uint32& TexId, const Rect& srcRect, const Sizef& destSize, const std::string& name = "" );

		/** Creates a SubTexture of the indicated part of the texture.
		*	@param TexId The texture id
		*	@param SrcRect The texture part that will be used as the SubTexture.
		*	@param DestSize The destination size that the SubTexture will have when rendered.
		*	@param Offset The offset that will be added to the position passed when any Draw call is used.
		*	@param Name The texture name ( if any )
		*/
		SubTexture( const Uint32& TexId, const Rect& srcRect, const Sizef& destSize, const Vector2i& offset, const std::string& name = "" );

		virtual ~SubTexture();

		/** @return The Texture Id that holds the SubTexture. */
		const Uint32& getTextureId();

		/** Set the Texture Id that holds the SubTexture. */
		void setTextureId( const Uint32& TexId );

		/** @return The Texture sector that represents the SubTexture */
		const Rect& getSrcRect() const;

		/** Sets the Texture sector that represents the SubTexture */
		void setSrcRect( const Rect& rect );

		/** @return The Destination Size of the SubTexture. */
		const Sizef& getDestSize() const;

		/** Sets the Destination Size of the SubTexture.
		*	The size can be different from the original size of the SubTexture.
		*	For example if the SubTexture width is 32 pixels, by default the destination width is 32 pixels, but it can be changed to anything want. */
		void setDestSize( const Sizef& destSize );

		/** @return The SubTexture default offset. The offset is added to the position passed when is drawed. */
		const Vector2i& getOffset() const;

		/** Set the SubTexture offset. */
		void setOffset( const Vector2i& offset );

		void draw( const Float& X, const Float& Y, const Color& color = Color::White, const Float& Angle = 0.f, const Vector2f& Scale = Vector2f::One, const EE_BLEND_MODE& Blend = ALPHA_NORMAL, const EE_RENDER_MODE& Effect = RN_NORMAL, OriginPoint Center = OriginPoint(OriginPoint::OriginCenter) );

		void draw( const Float& X, const Float& Y, const Float& Angle, const Vector2f& Scale, const Color& Color0 = Color::White, const Color& Color1 = Color::White, const Color& Color2 = Color::White, const Color& Color3 = Color::White, const EE_BLEND_MODE& Blend = ALPHA_NORMAL, const EE_RENDER_MODE& Effect = RN_NORMAL, OriginPoint Center = OriginPoint(OriginPoint::OriginCenter) );

		void draw( const Quad2f Q, const Vector2f& offset = Vector2f(), const Float& Angle = 0.f, const Vector2f& Scale = Vector2f::One, const Color& Color0 = Color::White, const Color& Color1 = Color::White, const Color& Color2 = Color::White, const Color& Color3 = Color::White, const EE_BLEND_MODE& Blend = ALPHA_NORMAL );

		virtual void draw();

		virtual void draw( const Vector2f& position );

		virtual void draw(const Vector2f& position, const Sizef & size );

		/** @return The texture instance used by the SubTexture. */
		Graphics::Texture * getTexture();

		/** Replaces a color in the SubTexture ( needs Lock() ) */
		void replaceColor( Color ColorKey, Color NewColor );

		/** Creates a mask from a color.  */
		void createMaskFromColor( Color ColorKey, Uint8 Alpha );

		/** Creates a mask from a color. */
		void createMaskFromColor( RGB ColorKey, Uint8 Alpha );

		/** Creates a copy of the alpha mask to memory from the texture loaded in VRAM. */
		void cacheAlphaMask();

		/** Creates a copy in memory from the texture loaded in VRAM.  */
		void cacheColors();

		/** @return The alpha value that corresponds to the position indicated in the SubTexture.
		*	If the SubTexture wasn't locked before this call, it will be locked automatically. */
		Uint8 getAlphaAt( const Int32& X, const Int32& Y );

		/** @return The color that corresponds to the position indicated in the SubTexture.
		*	If the SubTexture wasn't locked before this call, it will be locked automatically. */
		Color getColorAt( const Int32& X, const Int32& Y );

		/** @brief Set a color to the position indicated in the SubTexture.
		*	If the SubTexture wasn't locked before this call, it will be locked automatically.
		*/
		void setColorAt( const Int32& X, const Int32& Y, const Color& Color );

		/** Deletes the texture buffer from memory ( not from VRAM ) if it was cached before ( using Lock() ). */
		void clearCache();

		/** @brief Locks the texture to be able to perform read/write operations.
		*	@see Texture::Lock */
		Uint8 * lock();

		/** @brief Unlocks the current texture locked.
		*	@see Texture::Unlock */
		bool unlock( const bool& KeepData = false, const bool& Modified = false );

		/** @return The SubTexture size in the texture. This is the source rect size. */
		Sizei getRealSize();

		/** @return This is the same as Destination Size but with the values rounded as integers. */
		Sizef getSize();

		/** @return A pixel pointer to the texture loaded in memory ( downloaded from VRAM doing Lock()/Unlock() ). */
		const Uint8* getPixelsPtr();

		/** Saves the SubTexture to a file in the file format specified.
		*	This will get the Texture from VRAM ( it will not work with OpenGL ES ) */
		bool saveToFile( const std::string& filepath, const EE_SAVE_TYPE& Format );

		/** Sets the Destination Size as the Source Rect Size ( the real size of the SubTexture ) multiplied by the pixel density. */
		void resetDestSize();

		Float getPixelDensity() const;

		void setPixelDensity( const Float & pixelDensity );

		Sizei getDpSize();

		Sizei getPxSize();

		Sizef getOriDestSize() const;

		void setOriDestSize(const Sizef & oriDestSize);
	protected:
		Uint8 *		mPixels;
		Uint8 *		mAlphaMask;
		Uint32 		mTexId;
		Graphics::Texture * 	mTexture;
		Rect		mSrcRect;
		Sizef		mOriDestSize;
		Sizef		mDestSize;
		Vector2i	mOffset;
		Float		mPixelDensity;
};

}}

#endif
