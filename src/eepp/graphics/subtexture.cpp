#include <eepp/graphics/subtexture.hpp>
#include <eepp/graphics/texturefactory.hpp>
#include <eepp/graphics/renderer/renderer.hpp>
#include <eepp/helper/SOIL2/src/SOIL2/SOIL2.h>
#include <eepp/helper/jpeg-compressor/jpge.h>
#include <eepp/graphics/texturesaver.hpp>
#include <eepp/graphics/renderer/opengl.hpp>
using namespace EE::Graphics::Private;

namespace EE { namespace Graphics {

SubTexture::SubTexture() :
	DrawableResource( DRAWABLE_SUBTEXTURE ),
	mPixels(NULL),
	mAlphaMask(NULL),
	mTexId(0),
	mTexture(NULL),
	mSrcRect( Rect(0,0,0,0) ),
	mOriDestSize(0,0),
	mDestSize(0,0),
	mOffset(0,0),
	mPixelDensity(1)
{
}

SubTexture::SubTexture( const Uint32& TexId, const std::string& name ) :
	DrawableResource( DRAWABLE_SUBTEXTURE, name ),
	mPixels(NULL),
	mAlphaMask(NULL),
	mTexId( TexId ),
	mTexture( TextureFactory::instance()->getTexture( TexId ) ),
	mSrcRect( Rect( 0, 0, NULL != mTexture ? mTexture->getImageWidth() : 0, NULL != mTexture ? mTexture->getImageHeight() : 0 ) ),
	mOriDestSize( (Float)mSrcRect.getSize().getWidth(), (Float)mSrcRect.getSize().getHeight() ),
	mDestSize( mOriDestSize ),
	mOffset(0,0),
	mPixelDensity(1)
{
}

SubTexture::SubTexture( const Uint32& TexId, const Rect& SrcRect, const std::string& name ) :
	DrawableResource( DRAWABLE_SUBTEXTURE, name ),
	mPixels(NULL),
	mAlphaMask(NULL),
	mTexId( TexId ),
	mTexture( TextureFactory::instance()->getTexture( TexId ) ),
	mSrcRect( SrcRect ),
	mOriDestSize( (Float)( mSrcRect.Right - mSrcRect.Left ), (Float)( mSrcRect.Bottom - mSrcRect.Top ) ),
	mDestSize( mOriDestSize ),
	mOffset(0,0),
	mPixelDensity(1)
{
}

SubTexture::SubTexture( const Uint32& TexId, const Rect& SrcRect, const Sizef& DestSize, const std::string& name ) :
	DrawableResource( DRAWABLE_SUBTEXTURE, name ),
	mPixels(NULL),
	mAlphaMask(NULL),
	mTexId( TexId ),
	mTexture( TextureFactory::instance()->getTexture( TexId ) ),
	mSrcRect(SrcRect),
	mOriDestSize(DestSize),
	mDestSize(DestSize),
	mOffset(0,0),
	mPixelDensity(1)
{
}

SubTexture::SubTexture(const Uint32& TexId, const Rect& SrcRect, const Sizef& DestSize, const Vector2i &Offset, const std::string& name ) :
	DrawableResource( DRAWABLE_SUBTEXTURE, name ),
	mPixels(NULL),
	mAlphaMask(NULL),
	mTexId( TexId ),
	mTexture( TextureFactory::instance()->getTexture( TexId ) ),
	mSrcRect(SrcRect),
	mOriDestSize(DestSize),
	mDestSize(DestSize),
	mOffset(Offset),
	mPixelDensity(1)
{
}

SubTexture::~SubTexture() {
	clearCache();
}

const Uint32& SubTexture::getTextureId() {
	return mTexId;
}

void SubTexture::setTextureId( const Uint32& TexId ) {
	mTexId		= TexId;
	mTexture	= TextureFactory::instance()->getTexture( TexId );
}

const Rect& SubTexture::getSrcRect() const {
	return mSrcRect;
}

void SubTexture::setSrcRect( const Rect& rect ) {
	mSrcRect = rect;

	if ( NULL != mPixels )
		cacheColors();

	if ( NULL != mAlphaMask )
		cacheAlphaMask();
}

const Sizef& SubTexture::getDestSize() const {
	return mDestSize;
}

void SubTexture::setDestSize( const Sizef& destSize ) {
	mDestSize = destSize;

	if ( mOriDestSize.x == 0 && mOriDestSize.y == 0 ) {
		mOriDestSize = destSize;
	}
}

const Vector2i& SubTexture::getOffset() const {
	return mOffset;
}

void SubTexture::setOffset( const Vector2i& offset ) {
	mOffset = offset;
}

void SubTexture::draw( const Float& X, const Float& Y, const Color& Color, const Float& Angle, const Vector2f& Scale, const EE_BLEND_MODE& Blend, const EE_RENDER_MODE& Effect, OriginPoint Center ) {
	if ( NULL != mTexture )
		mTexture->drawEx( X + mOffset.x, Y + mOffset.y, mDestSize.x, mDestSize.y, Angle, Scale, Color, Color, Color, Color, Blend, Effect, Center, mSrcRect );
}

void SubTexture::draw( const Float& X, const Float& Y, const Float& Angle, const Vector2f& Scale, const Color& Color0, const Color& Color1, const Color& Color2, const Color& Color3, const EE_BLEND_MODE& Blend, const EE_RENDER_MODE& Effect, OriginPoint Center ) {
	if ( NULL != mTexture )
		mTexture->drawEx( X + mOffset.x, Y + mOffset.y, mDestSize.x, mDestSize.y, Angle, Scale, Color0, Color1, Color2, Color3, Blend, Effect, Center, mSrcRect );
}

void SubTexture::draw( const Quad2f Q, const Vector2f& Offset, const Float& Angle, const Vector2f& Scale, const Color& Color0, const Color& Color1, const Color& Color2, const Color& Color3, const EE_BLEND_MODE& Blend ) {
	if ( NULL != mTexture )
		mTexture->drawQuadEx( Q, Offset, Angle, Scale, Color0, Color1, Color2, Color3, Blend, mSrcRect );
}

void SubTexture::draw() {
	draw( mPosition );
}

void SubTexture::draw( const Vector2f& position ) {
	draw( position.x, position.y, getColor() );
}

void SubTexture::draw( const Vector2f & position, const Sizef& size ) {
	Sizef oldSize( mDestSize );
	mDestSize = size;
	draw( position.x, position.y, getColor() );
	mDestSize = oldSize;
}

Graphics::Texture * SubTexture::getTexture() {
	return mTexture;
}

void SubTexture::replaceColor( Color ColorKey, Color NewColor ) {
	mTexture->lock();

	for ( int y = mSrcRect.Top; y < mSrcRect.Bottom; y++ ) {
		for ( int x = mSrcRect.Left; x < mSrcRect.Right; x++ ) {
			if ( mTexture->getPixel( x, y ) == ColorKey )
				mTexture->setPixel( x, y, NewColor );
		}
	}

	mTexture->unlock( false, true );
}

void SubTexture::createMaskFromColor(Color ColorKey, Uint8 Alpha) {
	replaceColor( ColorKey, Color( ColorKey.r, ColorKey.g, ColorKey.b, Alpha ) );
}

void SubTexture::createMaskFromColor(RGB ColorKey, Uint8 Alpha) {
	createMaskFromColor( Color( ColorKey.r, ColorKey.g, ColorKey.b, 255 ), Alpha );
}

void SubTexture::cacheAlphaMask() {
	Uint32 size = ( mSrcRect.Right - mSrcRect.Left ) * ( mSrcRect.Bottom - mSrcRect.Top );

	eeSAFE_DELETE_ARRAY( mAlphaMask );
	mAlphaMask = eeNewArray( Uint8, size );

	mTexture->lock();

	int rY = 0;
	int rX = 0;
	int rW = mSrcRect.Right - mSrcRect.Left;

	for ( int y = mSrcRect.Top; y < mSrcRect.Bottom; y++ ) {
		rY = y - mSrcRect.Top;

		for ( int x = mSrcRect.Left; x < mSrcRect.Right; x++ ) {
			rX = x - mSrcRect.Left;

			mAlphaMask[ rX + rY * rW ] = mTexture->getPixel( x, y ).a;
		}
	}

	mTexture->unlock();
}

void SubTexture::cacheColors() {
	mTexture->lock();

	Uint32 size =  ( mSrcRect.Right - mSrcRect.Left ) * ( mSrcRect.Bottom - mSrcRect.Top ) * mTexture->getChannels();

	eeSAFE_DELETE_ARRAY( mPixels );

	mPixels = eeNewArray( Uint8, size );

	int rY = 0;
	int rX = 0;
	int rW = mSrcRect.Right - mSrcRect.Left;
	Color tColor;
	Uint32 Channels = mTexture->getChannels();
	int Pos;

	for ( int y = mSrcRect.Top; y < mSrcRect.Bottom; y++ ) {
		rY = y - mSrcRect.Top;

		for ( int x = mSrcRect.Left; x < mSrcRect.Right; x++ ) {
			rX = x - mSrcRect.Left;

			tColor = mTexture->getPixel( x, y );

			Pos = ( rX + rY * rW ) * Channels;

			if ( Channels >= 1 ) mPixels[ Pos ]		= tColor.r;
			if ( Channels >= 2 ) mPixels[ Pos + 1 ]	= tColor.g;
			if ( Channels >= 3 ) mPixels[ Pos + 2 ]	= tColor.b;
			if ( Channels >= 4 ) mPixels[ Pos + 3 ]	= tColor.a;
		}
	}

	mTexture->unlock();
}

Uint8 SubTexture::getAlphaAt( const Int32& X, const Int32& Y ) {
	if ( mTexture->hasLocalCopy() )
		return mTexture->getPixel( mSrcRect.Left + X, mSrcRect.Right + Y ).a;

	if ( NULL != mAlphaMask )
		return mAlphaMask[ X + Y * ( mSrcRect.Right - mSrcRect.Left ) ];

	if ( NULL != mPixels )
		return mPixels[ ( X + Y * ( mSrcRect.Right - mSrcRect.Left ) ) * mTexture->getChannels() + 3 ];

	cacheAlphaMask();

	return getAlphaAt( X, Y );
}

Color SubTexture::getColorAt( const Int32& X, const Int32& Y ) {
	if ( mTexture->hasLocalCopy() )
		return mTexture->getPixel( mSrcRect.Left + X, mSrcRect.Right + Y );

	if ( NULL != mPixels ) {
		Uint32 Channels = mTexture->getChannels();
		unsigned int Pos = ( X + Y * ( mSrcRect.Right - mSrcRect.Left ) ) * Channels;

		if ( 4 == Channels )
			return Color( mPixels[ Pos ], mPixels[ Pos + 1 ], mPixels[ Pos + 2 ], mPixels[ Pos + 3 ] );
		else if ( 3 == Channels )
			return Color( mPixels[ Pos ], mPixels[ Pos + 1 ], mPixels[ Pos + 2 ], 255 );
		else if ( 2 == Channels )
			return Color( mPixels[ Pos ], mPixels[ Pos + 1 ], 255, 255 );
		else
			return Color( mPixels[ Pos ], 255, 255, 255 );
	}

	cacheColors();

	return getColorAt( X, Y );
}

void SubTexture::setColorAt( const Int32& X, const Int32& Y, const Color& Color ) {
	if ( NULL != mPixels ) {
		Uint32 Channels = mTexture->getChannels();
		unsigned int Pos = ( X + Y * ( mSrcRect.Right - mSrcRect.Left ) ) * Channels;

		if ( Channels >= 1 ) mPixels[ Pos ]		= Color.r;
		if ( Channels >= 2 ) mPixels[ Pos + 1 ]	= Color.g;
		if ( Channels >= 3 ) mPixels[ Pos + 2 ]	= Color.b;
		if ( Channels >= 4 ) mPixels[ Pos + 3 ]	= Color.a;
	} else {
		cacheColors();
		setColorAt( X, Y, Color );
	}
}

void SubTexture::clearCache() {
	eeSAFE_DELETE_ARRAY( mPixels );
	eeSAFE_DELETE_ARRAY( mAlphaMask );
}

Uint8 * SubTexture::lock() {
	cacheColors();

	return &mPixels[0];
}

bool SubTexture::unlock( const bool& KeepData, const bool& Modified ) {
	if ( NULL != mPixels  && NULL != mTexture ) {
		if ( Modified ) {
			TextureSaver saver( mTexture->getHandle() );

			Uint32 Channels = mTexture->getChannels();
			Uint32 Channel = GL_RGBA;

			if ( 3 == Channels )
				Channel = GL_RGB;
			else if ( 2 == Channels )
				Channel = GL_LUMINANCE_ALPHA;
			else if ( 1 == Channels )
				Channel = GL_ALPHA;

			glTexSubImage2D( GL_TEXTURE_2D, 0, mSrcRect.Left, mSrcRect.Top, mSrcRect.getSize().getWidth(), mSrcRect.getSize().getHeight(), Channel, GL_UNSIGNED_BYTE, reinterpret_cast<const void *> ( &mPixels[0] ) );
		}

		if ( !KeepData ) {
			eeSAFE_DELETE_ARRAY( mPixels );
		}

		return true;
	}

	return false;
}

Sizei SubTexture::getRealSize() {
	return mSrcRect.getSize();
}

Sizef SubTexture::getSize() {
	return Sizef( (Float)((Int32)( mOriDestSize.getWidth() / mPixelDensity )), (Float)((Int32)( mOriDestSize.getHeight() / mPixelDensity )) );
}

const Uint8* SubTexture::getPixelsPtr() {
	if ( mPixels == NULL ) {
		lock();
		unlock(true);
	}

	return reinterpret_cast<const Uint8*> (&mPixels[0]);
}

bool SubTexture::saveToFile(const std::string& filepath, const EE_SAVE_TYPE& Format) {
	bool Res = false;

	lock();

	if ( NULL != mTexture ) {
		if ( SAVE_TYPE_JPG != Format ) {
			Res = 0 != ( SOIL_save_image ( filepath.c_str(), Format, getRealSize().getWidth(), getRealSize().getHeight(), mTexture->getChannels(), getPixelsPtr() ) );
		} else {
			jpge::params params;
			params.m_quality = Image::jpegQuality();
			Res = jpge::compress_image_to_jpeg_file( filepath.c_str(), getRealSize().getWidth(), getRealSize().getHeight(), mTexture->getChannels(), getPixelsPtr(), params);
		}
	}

	unlock();

	return Res;
}

void SubTexture::resetDestSize() {
	mDestSize.x	= mOriDestSize.getWidth();
	mDestSize.y = mOriDestSize.getHeight();
}

Float SubTexture::getPixelDensity() const {
	return mPixelDensity;
}

void SubTexture::setPixelDensity( const Float & pixelDensity ) {
	mPixelDensity = pixelDensity;
}

Sizei SubTexture::getDpSize() {
	return Sizei( (Int32)( mOriDestSize.getWidth() / mPixelDensity ), (Int32)( mOriDestSize.getHeight() / mPixelDensity ) );
}

Sizei SubTexture::getPxSize() {
	return Sizei( (Int32)( mOriDestSize.getWidth() / mPixelDensity * PixelDensity::getPixelDensity() ), (Int32)( mOriDestSize.getHeight() / mPixelDensity * PixelDensity::getPixelDensity() ) );
}

Sizef SubTexture::getOriDestSize() const {
	return mOriDestSize;
}

void SubTexture::setOriDestSize(const Sizef & oriDestSize) {
	mOriDestSize = oriDestSize;
}

}}
