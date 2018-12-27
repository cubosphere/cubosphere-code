/**
Copyright (C) 2010 Chriddo
Copyright (C) 2018 Valeri Ochinski <v19930312@gmail.com>

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/

#include <csetjmp>
#include "textures.hpp"
#include "globals.hpp"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#ifndef JPEG_TURBO
#include <jpeglib.h>
#else
#include <turbojpeg.h> // TODO: test is it required
using j_decompress_ptr = struct jpeg_decompress_struct*;
using boolean = bool;
#define TRUE true
#endif

#include <sstream>

#include <string.h>

#include <GL/glew.h>
#include <SDL.h>

#include "game.hpp"
#include "luautils.hpp"

using namespace std::string_view_literals;

///////////////////////////////////////////////

inline TextureType extToTexType(std::string_view ext) {
	if (ext == "jpg"sv or ext == "jpeg"sv) { return TextureType::JPEG; }
	else if (ext == "png"sv) { return TextureType::PNG; }
	else { return TextureType::Invalid; }
	}

inline std::unique_ptr<CuboFile> getTextrueFile(std::string subname,int type) {
	auto finfo=GetFileName(subname, type,".png"); // Try PNG first
	if (!finfo) { finfo=GetFileName(subname, type,".jpg"); }
	if (!finfo) { finfo=GetFileName(subname, type,".jpeg"); }
	return finfo;
	}

void TextureDef::Call_Render(int sideid) {
	lua.CallVAIfPresent("Render", {{sideid}});
	}

void TextureDef::RenderPlane() {
//OPEN GL STUFF
// glBegin(GL_QUADS);
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0,1,0);
	glTexCoord2f(0,0); glVertex3f(-0.5,0,-0.5);
	glTexCoord2f(0,1); glVertex3f(-0.5,0,0.5);

	glTexCoord2f(1,0); glVertex3f(0.5,0,-0.5);
	glTexCoord2f(1,1); glVertex3f(0.5,0,0.5);
	glEnd();
	}

void TextureDef::Render2d() {
//OPEN GL STUFF
//glBegin(GL_QUADS);
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0,0,-1);
	glTexCoord2f(0,1); glVertex3f(-0.5, -0.5, -10);
	glTexCoord2f(1,1); glVertex3f(0.5, -0.5, -10);
	glTexCoord2f(0,0); glVertex3f(-0.5, 0.5, -10);
	glTexCoord2f(1,0); glVertex3f(0.5, 0.5, -10);

	glEnd();


	}


///////////////////////////////////////////////

/* Read JPEG image from a memory segment */
static void init_source ([[maybe_unused]] j_decompress_ptr cinfo) {}
static boolean fill_input_buffer ([[maybe_unused]] j_decompress_ptr cinfo) {
//    ERREXIT(cinfo, JERR_INPUT_EMPTY);
	return TRUE;
	}
static void skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
	struct jpeg_source_mgr* src = (struct jpeg_source_mgr*) cinfo->src;

	if (num_bytes > 0) {
			src->next_input_byte += (size_t) num_bytes;
			src->bytes_in_buffer -= (size_t) num_bytes;
			}
	}
static void term_source ([[maybe_unused]] j_decompress_ptr cinfo) {}
static void jpeg_mem_src (j_decompress_ptr cinfo, void* buffer, long nbytes) {
	struct jpeg_source_mgr* src;

	if (cinfo->src == NULL) {   /* first time for this JPEG object? */
			cinfo->src = (struct jpeg_source_mgr *)
					(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
							sizeof(struct jpeg_source_mgr));
			}

	src = (struct jpeg_source_mgr*) cinfo->src;
	src->init_source = init_source;
	src->fill_input_buffer = fill_input_buffer;
	src->skip_input_data = skip_input_data;
	src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->term_source = term_source;
	src->bytes_in_buffer = nbytes;
	src->next_input_byte = (JOCTET*)buffer;
	}

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
	};

static void JPGFatalError(j_common_ptr cinfo) {

	my_error_mgr *myerr = (my_error_mgr*) cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	coutlog("Error in JPG",1);
	longjmp(myerr->setjmp_buffer, 1);
	}


bool JPEGTexture::loadFromFile(const std::unique_ptr<CuboFile>& finfo) {
	//  coutlog("Jpegloading not implemented yet");

	std::string fname=finfo->GetName();

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	FILE *infile=NULL;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	if (raw) { free(raw); } raw=NULL;

	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error( &(jerr.pub) );
	jerr.pub.error_exit = JPGFatalError;
	if (setjmp(jerr.setjmp_buffer)) {
			/* If we get here, the JPEG code has signaled an error.
			 * We need to clean up the JPEG object, close the input file, and return.
			 */
			jpeg_destroy_decompress(&cinfo);
			if (infile) { fclose(infile); }
			return false;
			}
	/* setup decompression process and source, then read JPEG header */
	jpeg_create_decompress( &cinfo );
	/* this makes the library read from infile */

	//coutlog("Try opening "+fname);



	if (finfo->IsHDDFile()) {
			infile = fopen( finfo->GetHDDName().c_str(), "rb" );
			if ( !infile ) {
					std::ostringstream os; os << "Error opening jpeg file " << fname ; coutlog(os.str(),2);
					return false;
					}
			jpeg_stdio_src( &cinfo, infile );

			}
	else {

			jpeg_mem_src(&cinfo, finfo->GetData()->data(), finfo->GetSize());

			}

	unsigned long location = 0;
	/* reading the image header which contains image information */
	jpeg_read_header( &cinfo, TRUE );
	jpeg_start_decompress( &cinfo );
	/* allocate memory to hold the uncompressed image */
	long int rawsize=cinfo.output_width*cinfo.output_height*cinfo.num_components;
	raw = (unsigned char*)malloc( rawsize );
	width=cinfo.output_width;
	height=cinfo.output_height;

	JSAMPROW row_pointer[1];
	row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );

// row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
//  buffer = (*cinfo.mem->alloc_sarray)
	//	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	/*
	ostringstream os; os << "Loading Texture: " << fname << endl << "    " << width << " x " << height << " x " << cinfo.num_components << "   -  " << rawsize << "  RAW-PTR: " << raw  ;
	 coutlog(os.str());*/
//cout << "Loading Texture " << fname << endl;


	while( cinfo.output_scanline < cinfo.image_height ) {
			jpeg_read_scanlines( &cinfo, row_pointer, 1 );
			for(unsigned int i=0; i<cinfo.image_width*cinfo.num_components; i++) {
					((JSAMPROW) raw)[location++] = row_pointer[0][i];
					}
			}

//for (unsigned int i=0;i<width*height*cinfo.num_components;i++) ((unsigned char *)(raw))[i]=rand();
	channels=cinfo.num_components;
	/* wrap up decompression, destroy objects, free pointers and close open files */
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	free( row_pointer[0] );

	if (finfo->IsHDDFile()) {
			fclose( infile );
			}
	else {}
	return true;
	}

bool PNGTexture::loadFromFile(const std::unique_ptr<CuboFile>& finfo) {
	try {
			auto img = std::make_unique<png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>>>(*finfo->GetStream());
			width = img->get_width();
			height = img->get_height();

			data = img->get_pixbuf().fetch_bytes();
			return true;
			}
	catch(png::error) {
			return false;
			}
	}


static int iabs(int x) {
	if (x<0) { return -x; }
	return x;
	}

int JPEGTexture::LoadAlphaTexture(const std::unique_ptr<CuboFile>& finfo) {
	std::string aname=finfo->GetName();

	auto alp = std::make_unique<JPEGTexture>();
	alp->loadFromFile(finfo);
	if (!(alp->getRGBPointer())) {return 0;}

	if (alp->GetChannels()!=1) { coutlog("Alpha-Texture "+aname+" is not a grayscale jpeg",2); return 0;}
	if (alp->getWidth()!=this->getWidth() ||  alp->getHeight()!=this->getHeight()) {
			std::ostringstream oss; oss <<"Alpha-Texture "+aname+" has wrong extents ("<< alp->getWidth()<< " x " <<alp->getHeight() <<")" << "! Must have " << getWidth() << " x " << getHeight();
			coutlog(oss.str(),2);
			return 0;
			}

	unsigned char *araw=(unsigned char*)(alp->getRGBPointer());
	unsigned char *newraw = (unsigned char*)malloc( getWidth()*getHeight()*4 );
	for (int p=0; p<getWidth()*getHeight(); p++) {
			int r=((unsigned char*)raw)[3*p];
			int g=((unsigned char*)raw)[3*p+1];
			int b=((unsigned char*)raw)[3*p+2];

			int a=araw[p];;
			//And store it back to the new buffer
			((unsigned char*)newraw)[4*p]=r;
			((unsigned char*)newraw)[4*p+1]=g;
			((unsigned char*)newraw)[4*p+2]=b;
			((unsigned char*)newraw)[4*p+3]=a;
			}

	free(raw);
	raw=newraw;
	trans=1;

	return 1;
	}


void JPEGTexture::ColorKeyTransparency(unsigned int ckey) {
	unsigned char *newraw = (unsigned char*)malloc( getWidth()*getHeight()*4 );
	ckey&=0xFFFFFF;

//Allow little disturbs
	int disturb=100;

	int cr=ckey & 0xFF;
	int cg=((ckey>>8) & 0xFF);
	int cb=((ckey>>16) & 0xFF);

	for (int p=0; p<getWidth()*getHeight(); p++) {
			int r=((unsigned char*)raw)[3*p];
			int g=((unsigned char*)raw)[3*p+1];
			int b=((unsigned char*)raw)[3*p+2];

			int a;

			if ( (iabs(cr-r)<disturb) && (iabs(cg-g)<disturb) && (iabs(cb-b)<disturb) ) { a=0; }

			else { a=255; }
			//And store it back to the new buffer
			((unsigned char*)newraw)[4*p]=r;
			((unsigned char*)newraw)[4*p+1]=g;
			((unsigned char*)newraw)[4*p+2]=b;
			((unsigned char*)newraw)[4*p+3]=a;
			}

	free(raw);
	raw=newraw;
	trans=1;
	}


static int isPowerOf2(int n) {
	return (n & (n - 1)) == 0 && n != 0;
	}

static uint32_t myLog2(uint32_t x) {
	uint32_t y;
	asm ( "\tbsr %1, %0\n"
			: "=r"(y)
			: "r" (x)
		);
	return y;
	}



void JPEGTexture::shrink_half_blur() {
	unsigned char *nraw_image = NULL;
	unsigned char *raw_image = (unsigned char *)raw;
	int neww=width>>1;
	int newh=height>>1;

	nraw_image=(unsigned char*)malloc( 3*neww*newh );
	int di=0;
	int sp=0;
	int offsy=3*width;
	for (int j=0; j<newh; j++) {
			for (int i=0; i<neww; i++) {
					auto nc=(raw_image[sp]+raw_image[sp+3]+raw_image[sp+offsy]+raw_image[sp+offsy+3])>>2;
					nraw_image[di]=nc;
					nc=(raw_image[sp+1]+raw_image[sp+3+1]+raw_image[sp+offsy+1]+raw_image[sp+offsy+3+1])>>2;
					nraw_image[di+1]=nc;
					nc=(raw_image[sp+2]+raw_image[sp+3+2]+raw_image[sp+offsy+2]+raw_image[sp+offsy+3+2])>>2;
					nraw_image[di+2]=raw_image[sp+2];
					sp+=3*2;
					di+=3;
					}
			sp+=offsy;
			}
	free(raw_image);
	raw=(void*)nraw_image;
	width=neww;
	height=newh;
	}

void JPEGTexture::shrink_blur(int ammount) {
	for (int i=0; i<ammount; i++) { shrink_half_blur(); }
	}

int JPEGTexture::CanFastResize(int maxdim) {
	return !HasAlpha() and isPowerOf2(width) and isPowerOf2(height) and (width==height) and (width>maxdim);
	}

void JPEGTexture::FastResize(int maxdim) {
	if (!CanFastResize(maxdim)) { return; }
//Ok, calc the ammount
	auto maxammount=myLog2(maxdim);
	auto thisammount=myLog2(width);
	auto ammount = thisammount-maxammount;
	shrink_blur(ammount);
	}

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

void TextureContainer::AddChar(int x,int y,void *data,int width,int fsize) {


	int max=width/fsize;
	FontExtend fe;
	fe.u1=(float)(x/fsize)/(float)max;
	fe.v1=(float)(y/fsize)/(float)max;
	fe.v2=(float)(y/fsize+0.99)/(float)max;




	float frac=0.99;

	for (int cx=fsize-2; cx>0; cx--) {
			int found=0;
			for (int cy=0; cy<fsize; cy++) {
					int p=(y+cy)*width+(x+cx);

					unsigned char a=((unsigned char *)data)[4*p+3];
					if (a) {
							frac=(float)(cx+1)/(float)fsize;
							found=1;
							break;
							}

					}
			if (found) { break; }
			}


	fe.u2=(float)(x/fsize+frac)/(float)max;

	chars.push_back(fe);
	}

void TextureContainer::makeFromTexture(Texture* texture,int asfont,int maxsize) {
	int clo=clock();
	int canfastresize=texture->CanFastResize(maxsize) and (!asfont);
	if (canfastresize) {
			texture->FastResize(maxsize);

			}
	void *data=texture->getRGBPointer();

	int width=texture->getWidth();
	int height=texture->getHeight();
	int scaled=0;
	if (  ((width>maxsize) || (height>maxsize)) && (!asfont) && (!canfastresize)) {
			//Rescale it
			int owidth=width;
			int oheight=height;
			GLenum type=GL_RGB;
			void *newdata;
			if (texture->HasAlpha()) { type=GL_RGBA; }
			if (width>maxsize) { width=maxsize; }
			if (height>maxsize) { height=maxsize; }
			newdata=malloc(width*height*(3+texture->HasAlpha()));
			gluScaleImage(type,owidth,oheight,GL_UNSIGNED_BYTE,data,width,height,GL_UNSIGNED_BYTE,newdata);
			data=newdata;
			scaled=1;
			}
	duration1=clock()-clo;


	if (asfont) {
			//Building the font with (asfont x asfont) sized letters
			//First the simplest approach:
			int maxy=texture->getHeight()/asfont;
			int maxx=texture->getWidth()/asfont;
			for (int y=0; y<maxy; y++) {
					for (int x=0; x<maxx; x++) {
							AddChar(x*asfont,y*asfont,data,texture->getWidth(),asfont);
							}
					}
			}

	glGenTextures( 1, &tind );

	glBindTexture( GL_TEXTURE_2D, tind );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
//ANIS
	if (strstr((char*)glGetString(GL_EXTENSIONS),
			"GL_EXT_texture_filter_anisotropic")) {
			float maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
			}

	clo=clock();

	if (!asfont) { // No Mipmaps for fonts
			if (!texture->HasAlpha())
				gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width,height,
						GL_RGB, GL_UNSIGNED_BYTE, data );
			else
				gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width,height,
						GL_RGBA, GL_UNSIGNED_BYTE, data );
			}
	else {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width,height, 0, GL_RGB,GL_UNSIGNED_BYTE, data);
			}
	duration2=clock()-clo;
	if (scaled) { free(data); }
	}


void TextureContainer::activate() {


	glBindTexture( GL_TEXTURE_2D, tind );
	}

void TextureContainer::activate(int stage) {
	if (g_Game()->HasGLSL() && (stage>0)) {
			glActiveTexture(GL_TEXTURE0+stage);
			}

	glBindTexture( GL_TEXTURE_2D, tind );

	if (g_Game()->HasGLSL() && (stage>0)) {
			glActiveTexture(GL_TEXTURE0);
			}
	}


void TextureServer::Invalidate() {
	for (unsigned int i=0; i<activetextures.size(); i++) { activetextures[i]=-2333; }
	}

void TextureServer::activate(int i) {
	if (activetextures.size()==0) { activetextures.push_back(i); }
	else if  (activetextures[0]==i) { return; }
	if (i<0) { DeactivateStage(0); }
	else { Textures[i].activate(); }
	}
void TextureServer::activate(int i,int stage) {
	if ((int)(activetextures.size())<=stage) { activetextures.resize(stage+1,-23255); }
	if  (activetextures[0]==i) { return; }

	if (i<0) { DeactivateStage(stage); }
	else { Textures[i].activate(stage); }
	}

FontExtend TextureServer::GetFontExtend(int index,int num) {
	return Textures[index].GetFontExtend(num);
	}

int TextureServer::TempTextureIndexFromName(std::string tname) {
	for (unsigned int i=0; i<filenames.size(); i++) if ((tname==filenames[i]) && (istemp[i]) ) { return i; }
	return -1;

	}

int TextureServer::LoadTempTexture(std::string tname, const std::unique_ptr<CuboFile>& finfo,int asfont,unsigned int colorkey) {
	int index=TempTextureIndexFromName(tname);
	TextureContainer *cont;
	if (index==-1) {
			Textures.resize(Textures.size()+1);
			index=Textures.size()-1;
			filenames.push_back(tname);
			alphanames.push_back(finfo->GetName());
			istemp.push_back(1);
			}
	else { //Load texture into existing sloz
			//Free the texture in this slot
			Textures[index].clear();
			}
	cont=&Textures[index];


	Texture *tex=NULL;
	std::string ext;
	std::string fname=finfo->GetName();
	auto extind = fname.rfind('.');
	if (extind != std::string::npos) {
			ext=fname.substr(extind+1);
			}
	else { ext=""; }
	auto texType = extToTexType(ext);
	switch(texType) {
			case TextureType::JPEG:
				tex = new JPEGTexture;
				break;
			case TextureType::PNG:
				tex = new PNGTexture;
				break;
			default:
				coutlog("Unknown texture type for "+fname+"\n", 2);
				return -1;
			};

	if (!tex) { coutlog("Failed to load "+fname+"\n", 2); return -1;}
	if (!tex->loadFromFile(finfo)) { coutlog("Failed to load "+fname+"\n", 2); return -1;}
	if (colorkey && texType == TextureType::JPEG) {
			((JPEGTexture *)tex)->ColorKeyTransparency(colorkey);
			}

	cont->makeFromTexture(tex,asfont,maxsize);

	delete tex;

	return index;
	}

int TextureServer::LoadTextureAndAlpha(const std::unique_ptr<CuboFile>& finfo,std::unique_ptr<CuboFile>& finfoa) { // JPEG only, deprecated
	std::string fname=finfo->GetName();
	std::string aname=finfoa->GetName();
	for (unsigned int i=0; i<filenames.size(); i++) if (fname==filenames[i] && aname==alphanames[i]) { return i; }
	if (g_VerboseMode()) {coutlog("Loading Texture: "+finfo->GetNameForLog());   coutlog("  -> using AlphaTexture: "+finfoa->GetNameForLog());}
//Find the right container Type
	Texture *tex=NULL;
	std::string ext;
	auto extind = fname.rfind('.');
	if (extind != std::string::npos) {
			ext=fname.substr(extind+1);
			}
	else { ext=""; }
	if (ext=="jpg" || ext=="jpeg") { tex=new JPEGTexture; }
	if (!tex) { std::ostringstream os; os << "Cannot load Texture "<< fname << std::endl; coutlog(os.str(),2); return -1;}
	int clo=clock();
	tex->loadFromFile(finfo);
	if (!(((JPEGTexture *)(tex))->LoadAlphaTexture(finfoa))) { std::ostringstream os; os << "Cannot load Alpha-Texture "<< aname << std::endl; coutlog(os.str(),2); delete tex; return -1;}
	timer1+=clock()-clo;
	int res=addTexture(tex,0);
	delete tex;
	numloads++;
	filenames.push_back(fname);
	alphanames.push_back(aname);
	istemp.push_back(0);
	return res;

	}

int TextureServer::LoadTexture(const std::unique_ptr<CuboFile>& finfo,int asfont,unsigned int colorkey) {
//Is it loaded already?
	std::string s=finfo->GetName();
	for (unsigned int i=0; i<filenames.size(); i++) if (s==filenames[i] && alphanames[i]=="") { return i; }
	if (g_VerboseMode()) { coutlog("Loading Texture: "+finfo->GetNameForLog()); }
//Find the right container Type
	Texture *tex=NULL;
	std::string ext;
	auto extind = s.rfind('.');
	if (extind != std::string::npos) {
			ext=s.substr(extind+1);
			}
	else { ext=""; }
	auto texType = extToTexType(ext);
	switch(texType) {
			case TextureType::JPEG:
				tex = new JPEGTexture;
				break;
			case TextureType::PNG:
				tex = new PNGTexture;
				break;
			default:
				coutlog("Unknown texture type for "+s+"\n", 2);
				return -1;
			};

	if (!tex) { std::ostringstream os; os << "Cannot load Texture "<< s << std::endl; coutlog(os.str(),2); return -1;}
	int clo=clock();
	tex->loadFromFile(finfo);
	timer1+=clock()-clo;
	if (colorkey && texType == TextureType::JPEG) {
			((JPEGTexture *)tex)->ColorKeyTransparency(colorkey);
			}
	int res=addTexture(tex,asfont);

	delete tex;

	numloads++;
	filenames.push_back(s);
	alphanames.push_back("");
	istemp.push_back(0);

	return res;

	}

void TextureServer::Reload() {
	for (unsigned int i=0; i<activetextures.size(); i++) { activate(-1,i); }

	for (unsigned int i=0; i<Textures.size(); i++) {
			Textures[i].clear();
			}

	Textures.clear();

	std::vector<std::string> ofilenames;
	for (unsigned int i=0; i<filenames.size(); i++) { ofilenames.push_back(filenames[i]); }
	filenames.clear();

	for (unsigned int i=0; i<ofilenames.size(); i++) {

			std::unique_ptr<CuboFile> finfo;

			std::string nam;
			if (istemp[i]) {
					nam=alphanames[i];
					}
			else {
					nam=ofilenames[i];
					}
			finfo=g_BaseFileSystem()->GetFileForReading(nam);



			if (!finfo) { coutlog("Texture "+nam+ " not found!",2); Textures.push_back(TextureContainer()); filenames.push_back(nam);  continue;}



			if (alphanames[i]=="" && (!istemp[i])) {
					LoadTexture(finfo,false);
					}
			else if (!istemp[i]) {
					std::unique_ptr<CuboFile> finfoa=g_BaseFileSystem()->GetFileForReading(alphanames[i]);
					if (!finfoa) {coutlog("Alpha Texture "+alphanames[i]+" not found!",2); Textures.push_back(TextureContainer()); filenames.push_back(ofilenames[i]);  continue; }
					LoadTextureAndAlpha(finfo,finfoa);
					}
			else {
					LoadTempTexture(ofilenames[i],finfo);
					}
			alphanames.pop_back();
			istemp.pop_back();

			//ostringstream oss; oss << "Storing texture " << i << " at " << r << "  fname :"  << filenames[i];
			//coutlog(oss.str(),2);

			}
	}

void TextureServer::clear() {

	for (unsigned int i=0; i<Textures.size(); i++) { Textures[i].clear(); }
	activetextures.clear();
	Textures.clear();
	filenames.clear();
	alphanames.clear();
	istemp.clear();
	ResetTimerCounters();
	}


void TextureServer::DeactivateStage(int stage) {
	if (g_Game()->HasGLSL() && (stage>0)) {
			glActiveTexture(GL_TEXTURE0+stage);
			}
	glBindTexture( GL_TEXTURE_2D, 0 );
	if (g_Game()->HasGLSL() && (stage>0)) {
			glActiveTexture(GL_TEXTURE0);
			}
	}


void TextureServer::ResetTimerCounters() {
	numloads=timer1=timer2=timer3=0;
	}

void TextureServer::CoutTimerString() {
	float t1=timer1/(float)CLOCKS_PER_SEC;
	float t2=timer2/(float)CLOCKS_PER_SEC;
	float t3=timer3/(float)CLOCKS_PER_SEC;
	std::ostringstream os;
	os << "Texture loading times: Loaded "<<numloads << " textures. " << std::endl;
	os <<"             Durations: Loading files (" << t1 << " s)" << std::endl;
	os <<"                        Resizing (" << t2 << " s)" << std::endl;
	os <<"                        Uploading/Mipmaps (" << t3 << " s)" << std::endl;
	coutlog(os.str());
	}




////////////LUA-IMPLEMENTATION///////////////////////////


int TEXDEF_Load(lua_State *state) {

	std::string name = LUA_GET_STRING(state);


	int r=g_Game()->GetLevel()->LoadTexDef(name);


	LUA_SET_NUMBER(state, r);

	return 1;
	}

int _TEXTDEF_ResetMeanwhile=0;

int TEXDEF_Render(lua_State *state) {
	int side= LUA_GET_INT(state);
	int tind= LUA_GET_INT(state);
	_TEXTDEF_ResetMeanwhile=1;
	g_Game()->GetLevel()->GetTexDef(tind)->Call_Render(side);
	if (_TEXTDEF_ResetMeanwhile==1)    {g_Game()->GetLevel()->SetLastRendered(g_Game()->GetLevel()->GetTexDef(tind)->GetName());}
	else {  _TEXTDEF_ResetMeanwhile=0;  g_Game()->GetLevel()->SetLastRendered(""); }

// g_Game()->GetLevel()->SetLastRendered("");
	return 0;
	}

int TEXDEF_RenderDirect(lua_State *state) {
	int tind= LUA_GET_INT(state);
	g_Game()->GetLevel()->GetTexDef(tind)->RenderPlane();
	return 0;
	}

int TEXDEF_Render2d(lua_State *state) {
	int tind= LUA_GET_INT(state);
	g_Game()->GetLevel()->GetTexDef(tind)->Render2d();
	return 0;
	}


int TEXDEF_GetLastRenderedType(lua_State *state) {
	std::string tn=g_Game()->GetLevel()->GetLastRendered();
	LUA_SET_STRING(state, tn);
	return 1;
	}

int TEXDEF_ResetLastRenderedType([[maybe_unused]] lua_State *state) {
	g_Game()->GetLevel()->SetLastRendered("");
	if (_TEXTDEF_ResetMeanwhile) { _TEXTDEF_ResetMeanwhile=2; }
	return 0;
	}


void LUA_TEXDEF_RegisterLib() {
	g_CuboLib()->AddFunc("TEXDEF_Load",TEXDEF_Load);
	g_CuboLib()->AddFunc("TEXDEF_Render",TEXDEF_Render);
	g_CuboLib()->AddFunc("TEXDEF_RenderDirect",TEXDEF_RenderDirect);
	g_CuboLib()->AddFunc("TEXDEF_Render2d",TEXDEF_Render2d);
	g_CuboLib()->AddFunc("TEXDEF_GetLastRenderedType",TEXDEF_GetLastRenderedType);
	g_CuboLib()->AddFunc("TEXDEF_ResetLastRenderedType",TEXDEF_ResetLastRenderedType);
	}



////////////////////////////////////////////////////////

int TEXTURE_LoadSkyTexture(lua_State *state) {
	std::string name = LUA_GET_STRING(state);
	auto finfo=getTextrueFile(name,FILE_SKYBOX);
	if (!finfo) {coutlog("SkyTexture "+name+ " not found!",2); LUA_SET_NUMBER(state, -1); return 1;}
	int r=g_Game()->GetTextures()->LoadTexture(finfo,false);
	LUA_SET_NUMBER(state, r);
	return 1;
	}

int TEXTURE_SetClamp(lua_State *state) {
	int mode=LUA_GET_INT(state);
	int m;
	if (mode==2) { m=GL_CLAMP_TO_EDGE; }
	else if (mode==1) { m=GL_CLAMP; }
	else { m=GL_REPEAT; }
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m);
	return 0;
	}


int TEXTURE_Load(lua_State *state) {
	//string name = lua_tostring(state, -1);
	//lua_pop(state,1);
	std::string name = LUA_GET_STRING(state);

	auto finfo=getTextrueFile(name,FILE_TEXTURE);
// coutlog("Loading Texture "+Texturename);
	if(finfo) {
			int r=g_Game()->GetTextures()->LoadTexture(finfo,false);
			LUA_SET_NUMBER(state, r);
			}
	else {
			LUA_SET_NUMBER(state, -1);
			}

	return 1;
	}


int TEXTURE_LoadWithAlpha(lua_State *state) {
	std::string aname = LUA_GET_STRING(state);
	std::string name = LUA_GET_STRING(state);

	auto finfo=GetFileName(name,FILE_TEXTURE,".jpg");
	if (!finfo) {coutlog("Texture "+name+ ".jpg not found!",2); LUA_SET_NUMBER(state, -1); return 1;}
	auto finfoa=GetFileName(aname,FILE_TEXTURE,".jpg");
	if (!finfoa) {coutlog("Alpha Texture "+aname+ ".jpg not found!",2); LUA_SET_NUMBER(state, -1); return 1;}

	int r=g_Game()->GetTextures()->LoadTextureAndAlpha(finfo,finfoa);
	LUA_SET_NUMBER(state, r);

	return 1;
	}

int TEXTURE_GetTempTexture(lua_State *state) {
	std::string tname=LUA_GET_STRING(state);
	int r=g_Game()->GetTextures()->TempTextureIndexFromName(tname);
	LUA_SET_NUMBER(state, r);
	return 1;
	}

int TEXTURE_LoadTempTexture(lua_State *state) {
	//string name = lua_tostring(state, -1);
	//lua_pop(state,1);

	std::string fname = LUA_GET_STRING(state);
	std::string tname= LUA_GET_STRING(state);
	auto cf=GetCuboFileFromRelativeName(fname);
	int r=0;
	if (cf) {
			r=g_Game()->GetTextures()->LoadTempTexture(tname,cf,false);
			}
	LUA_SET_NUMBER(state, r);

	return 1;
	}


int TEXTURE_SetMaxSize(lua_State *state) {
	int s=LUA_GET_INT(state);
	g_Game()->GetTextures()->SetMaxTextureSize(s);
	return 0;
	}


int TEXTURE_GetMaxSize(lua_State *state) {
	LUA_SET_NUMBER(state, g_Game()->GetTextures()->GetMaxTextureSize());
	return 1;
	}

int TEXTURE_Activate(lua_State *state) {
	int stage=LUA_GET_INT(state);
	int txt=LUA_GET_INT(state);
	g_Game()->GetTextures()->EnableTexturing();
	g_Game()->GetTextures()->activate(txt,stage);
	return 0;
	}

int TEXTURE_MatrixMode(lua_State *state) {
	int onoff=LUA_GET_INT(state);

	if (onoff) { glMatrixMode(GL_TEXTURE); }
	else { glMatrixMode(GL_MODELVIEW); }
	return 0;
	}

int TEXTURE_Deactivate(lua_State *state) {
	int stage=LUA_GET_INT(state);
//g_Game()->GetTextures()->EnableTexturing();
	g_Game()->GetTextures()->activate(-1,stage);
	return 0;
	}

void LUA_TEXTURE_RegisterLib() {
	g_CuboLib()->AddFunc("TEXTURE_Load",TEXTURE_Load);
	g_CuboLib()->AddFunc("TEXTURE_LoadWithAlpha",TEXTURE_LoadWithAlpha);
	g_CuboLib()->AddFunc("TEXTURE_Activate",TEXTURE_Activate);
	g_CuboLib()->AddFunc("TEXTURE_Deactivate",TEXTURE_Deactivate);
	g_CuboLib()->AddFunc("TEXTURE_SetMaxSize",TEXTURE_SetMaxSize);
	g_CuboLib()->AddFunc("TEXTURE_GetMaxSize",TEXTURE_GetMaxSize);
	g_CuboLib()->AddFunc("TEXTURE_LoadTempTexture",TEXTURE_LoadTempTexture);
	g_CuboLib()->AddFunc("TEXTURE_GetTempTexture",TEXTURE_GetTempTexture);
	g_CuboLib()->AddFunc("TEXTURE_MatrixMode",TEXTURE_MatrixMode);
	g_CuboLib()->AddFunc("TEXTURE_SetClamp",TEXTURE_SetClamp);
	g_CuboLib()->AddFunc("TEXTURE_LoadSkyTexture",TEXTURE_LoadSkyTexture);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
