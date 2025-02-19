#ifndef textureH
#define textureH
#include "corn/primitive.h"
#include "common/soil/USDA_texture.h"
namespace CS {
//______________________________________________________________________________
class Soil_texture
{
   float32  percent_sand;
   float32  percent_clay;
   float32  percent_silt;
   // Eventually a percent gravel might be determined from the texture qualifier
   USDA_texture::Texture_code    texture_code;
   USDA_texture::Texture_code    texture_qualifier;
public:
   Soil_texture(float32 _sand, float32 _clay, float32 _silt = 0);
   Soil_texture
      ( USDA_texture::Texture_code    texture_code
      , USDA_texture::Texture_code    texture_qualifier
      , nat8                 low_clay  = 0
      , nat8                 high_clay = 0
      );
      /**< USDA soil databases often provide the range of observed values for clay,
      this can be used to refine the texture percentage approximation.
      **/
   Soil_texture(const char *texture_code_label);
      /**< The texture code label may have a qualifier texture code
      and primary texture code separated by -.
      These will be extracted and the codes set to the soil texture polygon centroid.
      **/
   inline USDA_texture::Texture_code get_code()                            const { return texture_code; }  //060131
   inline float32 get_sand()                                               const { return percent_sand; }
   inline float32 get_clay()                                               const { return percent_clay; }
   inline float32 get_silt()                                               const { return percent_silt; }
   bool is_root_penetratable()                                             const;
private:
   void determine_texture();
   void determine_percentages
      ( nat8                 low_clay  = 0
      , nat8                 high_clay = 0);
      /**<  USDA soil databases often provide the range of observed values for
          clay, this can be used to refine the texture percentage approximation
         (This option has not yet been implemented)
      */
};
//_class Soil_texture_______________________________________________2000-01-14_/
}//_namespace CS_______________________________________________________________/
#endif

