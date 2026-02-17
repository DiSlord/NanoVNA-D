/*
 * Copyright (c) 2019-2026, Dmitry (DiSlord) dislordlive@gmail.com
 * All rights reserved.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * The software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include "nanovna.h"
#include <stdint.h>

/*
 * float has about 7.2 digits of precision
 * built sin table in range 0 to PI/2, use indexes from 0 to (1<<SIN_TABLE_N)/4
	for (int i = 0; i <= (1<<SIN_TABLE_N)/4; i++) {
		printf("% .9ff,%c", sinf(2 * M_PI * i / ((1<<SIN_TABLE_N)/4)), i % 8 == 7 ? '\n' : ' ');
	}
*/
const float sin_table_2048[2048/4 + 1] = { // Max ~1.2e-6 error on sin/cos calculation, ~5e-7 average error
	 0.000000000f,  0.003067957f,  0.006135885f,  0.009203754f,  0.012271538f,  0.015339206f,  0.018406730f,  0.021474080f,
	 0.024541229f,  0.027608145f,  0.030674802f,  0.033741172f,  0.036807224f,  0.039872929f,  0.042938255f,  0.046003181f,
	 0.049067676f,  0.052131705f,  0.055195246f,  0.058258265f,  0.061320737f,  0.064382628f,  0.067443915f,  0.070504576f,
	 0.073564567f,  0.076623864f,  0.079682440f,  0.082740262f,  0.085797310f,  0.088853553f,  0.091908954f,  0.094963498f,
	 0.098017141f,  0.101069860f,  0.104121633f,  0.107172422f,  0.110222206f,  0.113270953f,  0.116318636f,  0.119365208f,
	 0.122410670f,  0.125454977f,  0.128498107f,  0.131540030f,  0.134580702f,  0.137620121f,  0.140658244f,  0.143695042f,
	 0.146730468f,  0.149764523f,  0.152797192f,  0.155828401f,  0.158858150f,  0.161886394f,  0.164913118f,  0.167938292f,
	 0.170961887f,  0.173983872f,  0.177004218f,  0.180022910f,  0.183039889f,  0.186055139f,  0.189068660f,  0.192080393f,
	 0.195090324f,  0.198098406f,  0.201104626f,  0.204108968f,  0.207111374f,  0.210111842f,  0.213110313f,  0.216106802f,
	 0.219101235f,  0.222093612f,  0.225083917f,  0.228072077f,  0.231058121f,  0.234041959f,  0.237023592f,  0.240003020f,
	 0.242980182f,  0.245955050f,  0.248927608f,  0.251897812f,  0.254865646f,  0.257831097f,  0.260794133f,  0.263754696f,
	 0.266712755f,  0.269668311f,  0.272621363f,  0.275571793f,  0.278519690f,  0.281464934f,  0.284407556f,  0.287347466f,
	 0.290284663f,  0.293219179f,  0.296150863f,  0.299079835f,  0.302005947f,  0.304929256f,  0.307849646f,  0.310767144f,
	 0.313681751f,  0.316593379f,  0.319502026f,  0.322407693f,  0.325310290f,  0.328209847f,  0.331106305f,  0.333999664f,
	 0.336889833f,  0.339776874f,  0.342660725f,  0.345541298f,  0.348418683f,  0.351292759f,  0.354163527f,  0.357030958f,
	 0.359895051f,  0.362755746f,  0.365612984f,  0.368466824f,  0.371317208f,  0.374164075f,  0.377007395f,  0.379847199f,
	 0.382683456f,  0.385516047f,  0.388345033f,  0.391170382f,  0.393992037f,  0.396809995f,  0.399624199f,  0.402434677f,
	 0.405241311f,  0.408044159f,  0.410843194f,  0.413638294f,  0.416429549f,  0.419216901f,  0.422000289f,  0.424779683f,
	 0.427555084f,  0.430326492f,  0.433093816f,  0.435857087f,  0.438616246f,  0.441371292f,  0.444122136f,  0.446868837f,
	 0.449611336f,  0.452349573f,  0.455083579f,  0.457813323f,  0.460538685f,  0.463259786f,  0.465976506f,  0.468688846f,
	 0.471396714f,  0.474100202f,  0.476799250f,  0.479493737f,  0.482183754f,  0.484869242f,  0.487550169f,  0.490226507f,
	 0.492898166f,  0.495565236f,  0.498227656f,  0.500885367f,  0.503538370f,  0.506186664f,  0.508830190f,  0.511468828f,
	 0.514102757f,  0.516731799f,  0.519356012f,  0.521975279f,  0.524589717f,  0.527199149f,  0.529803574f,  0.532403111f,
	 0.534997642f,  0.537587106f,  0.540171504f,  0.542750776f,  0.545324981f,  0.547894061f,  0.550457954f,  0.553016722f,
	 0.555570245f,  0.558118522f,  0.560661614f,  0.563199341f,  0.565731764f,  0.568258941f,  0.570780754f,  0.573297143f,
	 0.575808227f,  0.578313828f,  0.580814004f,  0.583308637f,  0.585797846f,  0.588281572f,  0.590759695f,  0.593232334f,
	 0.595699310f,  0.598160684f,  0.600616455f,  0.603066564f,  0.605511010f,  0.607949793f,  0.610382795f,  0.612810075f,
	 0.615231574f,  0.617647290f,  0.620057225f,  0.622461259f,  0.624859512f,  0.627251804f,  0.629638255f,  0.632018745f,
	 0.634393275f,  0.636761844f,  0.639124453f,  0.641481042f,  0.643831551f,  0.646176040f,  0.648514390f,  0.650846660f,
	 0.653172851f,  0.655492842f,  0.657806695f,  0.660114348f,  0.662415802f,  0.664710939f,  0.666999936f,  0.669282556f,
	 0.671558976f,  0.673829019f,  0.676092744f,  0.678350091f,  0.680601001f,  0.682845533f,  0.685083687f,  0.687315345f,
	 0.689540565f,  0.691759288f,  0.693971455f,  0.696177125f,  0.698376238f,  0.700568795f,  0.702754736f,  0.704934061f,
	 0.707106769f,  0.709272802f,  0.711432159f,  0.713584840f,  0.715730846f,  0.717870057f,  0.720002532f,  0.722128212f,
	 0.724247038f,  0.726359129f,  0.728464365f,  0.730562747f,  0.732654274f,  0.734738886f,  0.736816585f,  0.738887310f,
	 0.740951121f,  0.743007958f,  0.745057762f,  0.747100592f,  0.749136388f,  0.751165152f,  0.753186762f,  0.755201340f,
	 0.757208824f,  0.759209216f,  0.761202395f,  0.763188422f,  0.765167296f,  0.767138898f,  0.769103348f,  0.771060526f,
	 0.773010433f,  0.774953127f,  0.776888490f,  0.778816521f,  0.780737221f,  0.782650590f,  0.784556568f,  0.786455214f,
	 0.788346410f,  0.790230215f,  0.792106569f,  0.793975472f,  0.795836926f,  0.797690868f,  0.799537301f,  0.801376164f,
	 0.803207517f,  0.805031300f,  0.806847513f,  0.808656156f,  0.810457170f,  0.812250614f,  0.814036310f,  0.815814435f,
	 0.817584813f,  0.819347501f,  0.821102500f,  0.822849810f,  0.824589312f,  0.826321065f,  0.828045070f,  0.829761207f,
	 0.831469595f,  0.833170176f,  0.834862888f,  0.836547732f,  0.838224709f,  0.839893818f,  0.841554940f,  0.843208253f,
	 0.844853580f,  0.846490920f,  0.848120332f,  0.849741757f,  0.851355195f,  0.852960646f,  0.854557991f,  0.856147349f,
	 0.857728601f,  0.859301805f,  0.860866904f,  0.862423956f,  0.863972843f,  0.865513623f,  0.867046237f,  0.868570685f,
	 0.870086968f,  0.871595085f,  0.873094976f,  0.874586642f,  0.876070142f,  0.877545297f,  0.879012227f,  0.880470872f,
	 0.881921232f,  0.883363307f,  0.884797096f,  0.886222541f,  0.887639642f,  0.889048338f,  0.890448749f,  0.891840696f,
	 0.893224299f,  0.894599497f,  0.895966291f,  0.897324622f,  0.898674428f,  0.900015891f,  0.901348829f,  0.902673304f,
	 0.903989315f,  0.905296743f,  0.906595707f,  0.907886147f,  0.909168005f,  0.910441279f,  0.911706030f,  0.912962198f,
	 0.914209783f,  0.915448666f,  0.916679025f,  0.917900741f,  0.919113815f,  0.920318246f,  0.921514034f,  0.922701120f,
	 0.923879504f,  0.925049245f,  0.926210225f,  0.927362561f,  0.928506076f,  0.929640889f,  0.930767000f,  0.931884229f,
	 0.932992756f,  0.934092522f,  0.935183525f,  0.936265647f,  0.937339008f,  0.938403547f,  0.939459205f,  0.940506101f,
	 0.941544056f,  0.942573190f,  0.943593442f,  0.944604874f,  0.945607364f,  0.946600914f,  0.947585583f,  0.948561311f,
	 0.949528158f,  0.950486064f,  0.951435030f,  0.952374995f,  0.953306019f,  0.954228103f,  0.955141187f,  0.956045270f,
	 0.956940353f,  0.957826436f,  0.958703458f,  0.959571481f,  0.960430503f,  0.961280465f,  0.962121427f,  0.962953269f,
	 0.963776052f,  0.964589775f,  0.965394437f,  0.966189981f,  0.966976464f,  0.967753828f,  0.968522131f,  0.969281256f,
	 0.970031261f,  0.970772147f,  0.971503854f,  0.972226501f,  0.972939968f,  0.973644257f,  0.974339366f,  0.975025356f,
	 0.975702107f,  0.976369739f,  0.977028131f,  0.977677345f,  0.978317380f,  0.978948176f,  0.979569733f,  0.980182111f,
	 0.980785251f,  0.981379211f,  0.981963873f,  0.982539296f,  0.983105481f,  0.983662426f,  0.984210074f,  0.984748483f,
	 0.985277653f,  0.985797524f,  0.986308098f,  0.986809433f,  0.987301409f,  0.987784147f,  0.988257587f,  0.988721669f,
	 0.989176512f,  0.989621997f,  0.990058184f,  0.990485072f,  0.990902662f,  0.991310835f,  0.991709769f,  0.992099345f,
	 0.992479563f,  0.992850423f,  0.993211925f,  0.993564129f,  0.993906975f,  0.994240463f,  0.994564593f,  0.994879305f,
	 0.995184720f,  0.995480776f,  0.995767415f,  0.996044695f,  0.996312618f,  0.996571124f,  0.996820331f,  0.997060061f,
	 0.997290432f,  0.997511446f,  0.997723043f,  0.997925282f,  0.998118103f,  0.998301566f,  0.998475552f,  0.998640239f,
	 0.998795450f,  0.998941302f,  0.999077737f,  0.999204755f,  0.999322414f,  0.999430597f,  0.999529421f,  0.999618828f,
	 0.999698818f,  0.999769390f,  0.999830604f,  0.999882340f,  0.999924719f,  0.999957621f,  0.999981165f,  0.999995291f,
	 1.000000000f
};

const float sin_table_1024[1024/4 + 1] = {
	 0.000000000f,  0.006135885f,  0.012271538f,  0.018406730f,  0.024541229f,  0.030674802f,  0.036807224f,  0.042938255f,
	 0.049067676f,  0.055195246f,  0.061320737f,  0.067443915f,  0.073564567f,  0.079682440f,  0.085797310f,  0.091908954f,
	 0.098017141f,  0.104121633f,  0.110222206f,  0.116318636f,  0.122410670f,  0.128498107f,  0.134580702f,  0.140658244f,
	 0.146730468f,  0.152797192f,  0.158858150f,  0.164913118f,  0.170961887f,  0.177004218f,  0.183039889f,  0.189068660f,
	 0.195090324f,  0.201104626f,  0.207111374f,  0.213110313f,  0.219101235f,  0.225083917f,  0.231058121f,  0.237023592f,
	 0.242980182f,  0.248927608f,  0.254865646f,  0.260794133f,  0.266712755f,  0.272621363f,  0.278519690f,  0.284407556f,
	 0.290284663f,  0.296150863f,  0.302005947f,  0.307849646f,  0.313681751f,  0.319502026f,  0.325310290f,  0.331106305f,
	 0.336889833f,  0.342660725f,  0.348418683f,  0.354163527f,  0.359895051f,  0.365612984f,  0.371317208f,  0.377007395f,
	 0.382683456f,  0.388345033f,  0.393992037f,  0.399624199f,  0.405241311f,  0.410843194f,  0.416429549f,  0.422000289f,
	 0.427555084f,  0.433093816f,  0.438616246f,  0.444122136f,  0.449611336f,  0.455083579f,  0.460538685f,  0.465976506f,
	 0.471396714f,  0.476799250f,  0.482183754f,  0.487550169f,  0.492898166f,  0.498227656f,  0.503538370f,  0.508830190f,
	 0.514102757f,  0.519356012f,  0.524589717f,  0.529803574f,  0.534997642f,  0.540171504f,  0.545324981f,  0.550457954f,
	 0.555570245f,  0.560661614f,  0.565731764f,  0.570780754f,  0.575808227f,  0.580814004f,  0.585797846f,  0.590759695f,
	 0.595699310f,  0.600616455f,  0.605511010f,  0.610382795f,  0.615231574f,  0.620057225f,  0.624859512f,  0.629638255f,
	 0.634393275f,  0.639124453f,  0.643831551f,  0.648514390f,  0.653172851f,  0.657806695f,  0.662415802f,  0.666999936f,
	 0.671558976f,  0.676092744f,  0.680601001f,  0.685083687f,  0.689540565f,  0.693971455f,  0.698376238f,  0.702754736f,
	 0.707106769f,  0.711432159f,  0.715730846f,  0.720002532f,  0.724247038f,  0.728464365f,  0.732654274f,  0.736816585f,
	 0.740951121f,  0.745057762f,  0.749136388f,  0.753186762f,  0.757208824f,  0.761202395f,  0.765167296f,  0.769103348f,
	 0.773010433f,  0.776888490f,  0.780737221f,  0.784556568f,  0.788346410f,  0.792106569f,  0.795836926f,  0.799537301f,
	 0.803207517f,  0.806847513f,  0.810457170f,  0.814036310f,  0.817584813f,  0.821102500f,  0.824589312f,  0.828045070f,
	 0.831469595f,  0.834862888f,  0.838224709f,  0.841554940f,  0.844853580f,  0.848120332f,  0.851355195f,  0.854557991f,
	 0.857728601f,  0.860866904f,  0.863972843f,  0.867046237f,  0.870086968f,  0.873094976f,  0.876070142f,  0.879012227f,
	 0.881921232f,  0.884797096f,  0.887639642f,  0.890448749f,  0.893224299f,  0.895966291f,  0.898674428f,  0.901348829f,
	 0.903989315f,  0.906595707f,  0.909168005f,  0.911706030f,  0.914209783f,  0.916679025f,  0.919113815f,  0.921514034f,
	 0.923879504f,  0.926210225f,  0.928506076f,  0.930767000f,  0.932992756f,  0.935183525f,  0.937339008f,  0.939459205f,
	 0.941544056f,  0.943593442f,  0.945607364f,  0.947585583f,  0.949528158f,  0.951435030f,  0.953306019f,  0.955141187f,
	 0.956940353f,  0.958703458f,  0.960430503f,  0.962121427f,  0.963776052f,  0.965394437f,  0.966976464f,  0.968522131f,
	 0.970031261f,  0.971503854f,  0.972939968f,  0.974339366f,  0.975702107f,  0.977028131f,  0.978317380f,  0.979569733f,
	 0.980785251f,  0.981963873f,  0.983105481f,  0.984210074f,  0.985277653f,  0.986308098f,  0.987301409f,  0.988257587f,
	 0.989176512f,  0.990058184f,  0.990902662f,  0.991709769f,  0.992479563f,  0.993211925f,  0.993906975f,  0.994564593f,
	 0.995184720f,  0.995767415f,  0.996312618f,  0.996820331f,  0.997290432f,  0.997723043f,  0.998118103f,  0.998475552f,
	 0.998795450f,  0.999077737f,  0.999322414f,  0.999529421f,  0.999698818f,  0.999830604f,  0.999924719f,  0.999981165f,
	 1.000000000f
};

const float sin_table_512[512/4 + 1] = {
	 0.000000000f,  0.012271538f,  0.024541229f,  0.036807224f,  0.049067676f,  0.061320737f,  0.073564567f,  0.085797310f,
	 0.098017141f,  0.110222206f,  0.122410670f,  0.134580702f,  0.146730468f,  0.158858150f,  0.170961887f,  0.183039889f,
	 0.195090324f,  0.207111374f,  0.219101235f,  0.231058121f,  0.242980182f,  0.254865646f,  0.266712755f,  0.278519690f,
	 0.290284663f,  0.302005947f,  0.313681751f,  0.325310290f,  0.336889833f,  0.348418683f,  0.359895051f,  0.371317208f,
	 0.382683456f,  0.393992037f,  0.405241311f,  0.416429549f,  0.427555084f,  0.438616246f,  0.449611336f,  0.460538685f,
	 0.471396714f,  0.482183754f,  0.492898166f,  0.503538370f,  0.514102757f,  0.524589717f,  0.534997642f,  0.545324981f,
	 0.555570245f,  0.565731764f,  0.575808227f,  0.585797846f,  0.595699310f,  0.605511010f,  0.615231574f,  0.624859512f,
	 0.634393275f,  0.643831551f,  0.653172851f,  0.662415802f,  0.671558976f,  0.680601001f,  0.689540565f,  0.698376238f,
	 0.707106769f,  0.715730846f,  0.724247038f,  0.732654274f,  0.740951121f,  0.749136388f,  0.757208824f,  0.765167296f,
	 0.773010433f,  0.780737221f,  0.788346410f,  0.795836926f,  0.803207517f,  0.810457170f,  0.817584813f,  0.824589312f,
	 0.831469595f,  0.838224709f,  0.844853580f,  0.851355195f,  0.857728601f,  0.863972843f,  0.870086968f,  0.876070142f,
	 0.881921232f,  0.887639642f,  0.893224299f,  0.898674428f,  0.903989315f,  0.909168005f,  0.914209783f,  0.919113815f,
	 0.923879504f,  0.928506076f,  0.932992756f,  0.937339008f,  0.941544056f,  0.945607364f,  0.949528158f,  0.953306019f,
	 0.956940353f,  0.960430503f,  0.963776052f,  0.966976464f,  0.970031261f,  0.972939968f,  0.975702107f,  0.978317380f,
	 0.980785251f,  0.983105481f,  0.985277653f,  0.987301409f,  0.989176512f,  0.990902662f,  0.992479563f,  0.993906975f,
	 0.995184720f,  0.996312618f,  0.997290432f,  0.998118103f,  0.998795450f,  0.999322414f,  0.999698818f,  0.999924719f,
	 1.000000000f
};

const float sin_table_256[256/4 + 1] = {
	 0.000000000f,  0.024541229f,  0.049067676f,  0.073564567f,  0.098017141f,  0.122410670f,  0.146730468f,  0.170961887f,
	 0.195090324f,  0.219101235f,  0.242980182f,  0.266712755f,  0.290284663f,  0.313681751f,  0.336889833f,  0.359895051f,
	 0.382683456f,  0.405241311f,  0.427555084f,  0.449611336f,  0.471396714f,  0.492898166f,  0.514102757f,  0.534997642f,
	 0.555570245f,  0.575808227f,  0.595699310f,  0.615231574f,  0.634393275f,  0.653172851f,  0.671558976f,  0.689540565f,
	 0.707106769f,  0.724247038f,  0.740951121f,  0.757208824f,  0.773010433f,  0.788346410f,  0.803207517f,  0.817584813f,
	 0.831469595f,  0.844853580f,  0.857728601f,  0.870086968f,  0.881921232f,  0.893224299f,  0.903989315f,  0.914209783f,
	 0.923879504f,  0.932992756f,  0.941544056f,  0.949528158f,  0.956940353f,  0.963776052f,  0.970031261f,  0.975702107f,
	 0.980785251f,  0.985277653f,  0.989176512f,  0.992479563f,  0.995184720f,  0.997290432f,  0.998795450f,  0.999698818f,
	 1.000000000f
};

// Define SIN table used in FFT and in sin/cos calculations
// full table size = 1<<SIN_TABLE_N, but in calculations use only first sector
//         !!! FFT_N must be <= SIN_TABLE_N !!!
// FFT_SIZE = 1 << FFT_N
#if   FFT_SIZE == 256 // Use bigger SIN table, provide less error in sin/cos calculations
#define SIN_TABLE   sin_table_512
#define SIN_TABLE_N  9
#define FFT_N        8
#elif FFT_SIZE == 512
#define SIN_TABLE   sin_table_512
#define SIN_TABLE_N  9
#define FFT_N        9
#elif FFT_SIZE == 1024
#define SIN_TABLE   sin_table_1024
#define SIN_TABLE_N 10
#define FFT_N       10
#elif FFT_SIZE == 2048
#define SIN_TABLE   sin_table_2048
#define SIN_TABLE_N 11
#define FFT_N       11
#else
#error "Need build table for new FFT size"
#endif

#if FFT_N > SIN_TABLE_N
  #error "Need use bigger SIN table for this FFT/IFFT"
#endif

#ifdef ARM_MATH_CM4
// Use CORTEX M4 rbit instruction (reverse bit order in 32bit value)
static uint32_t reverse_bits(uint32_t x, int n) {
  uint32_t result;
   __asm volatile ("rbit %0, %1" : "=r" (result) : "r" (x) );
  return result>>(32-n); // made shift for correct result
}
#elif 1 // Use shifts
static uint32_t reverse_bits(uint32_t x, int n) { // up to 16 bit
  x = ((x & 0x5555) << 1) | ((x & 0xAAAA) >> 1);
  x = ((x & 0x3333) << 2) | ((x & 0xCCCC) >> 2);
  x = ((x & 0x0F0F) << 4) | ((x & 0xF0F0) >> 4);
  x = ((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8);
  return x>>(16-n);
}
#elif 1 // Use table
static uint16_t reverse_bits(uint16_t x, int n) { // up to 12 bit
  static const uint8_t rev_nibble[16] = {0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 0x1, 0x9, 0x3, 0xB, 0x5, 0xD, 0x7, 0xF};
  x = (rev_nibble[(x >>  0) & 0xF] << 8) |
      (rev_nibble[(x >>  4) & 0xF] << 4) |
      (rev_nibble[(x >>  8) & 0xF] << 0);
  return x>>(12-n);
}
#else // Direct calculations
static uint16_t reverse_bits(uint16_t x, int n) {
  uint16_t result = 0;
  int i;
  for (i = 0; i < n; i++, x >>= 1)
    result = (result << 1) | (x & 1U);
  return result;
}
#endif

// Cooley-Tukey radix-2 DIT FFT, dir = 0:forward, 1:inverse
void fft(float array[][2], const uint8_t dir) {
  uint16_t fft_n    = FFT_N;     // can be in range 1 <= fft_n <= SIN_TABLE_N
  uint16_t fft_size = 1<<fft_n;  // data size = 1<<fft_n
  uint16_t i, j, k, l;
  for (i = 0; i < fft_size; i++) {
    if ((j = reverse_bits(i, fft_n)) > i) {
      SWAP(float, array[i][0], array[j][0]);
      SWAP(float, array[i][1], array[j][1]);
    }
  }
  // Optimized Cooley-Tukey decimation-in-time radix-2 FFT
  // Use SIN table (only first period), table size (1<<SIN_TABLE_N)/4
  uint16_t size = 1;
  uint16_t tablestep = (1<<SIN_TABLE_N) / 2;
  for (;size < fft_size; tablestep>>=1, size<<=1) {
    for (i = 0; i < size; i++) {
      uint32_t table_index = i * tablestep;
      const uint32_t SIN_TABLE_SUB = SIN_TABLE_N - 2;               // SIN table one sector N (full table must contain 4 sectors)
      const uint32_t sector = table_index >> SIN_TABLE_SUB;
      const uint32_t sidx = table_index & ((1<<SIN_TABLE_SUB) - 1); // sin value index
      const uint32_t cidx = (1<<SIN_TABLE_SUB) - sidx;              // cos value index
      const float sin = SIN_TABLE[sidx];
      const float cos = SIN_TABLE[cidx];
      float s = sector & 1 ?  cos : sin;                            // Fix digit and values for 90 ... 180 degree
      float c = sector & 1 ? -sin : cos;
      if (!dir) s= -s;
      for (k = i; k < fft_size; k = l + size) {
        l = k + size;
        float tpre = array[l][0] * c - array[l][1] * s;
        float tpim = array[l][0] * s + array[l][1] * c;
        array[l][0] = array[k][0] - tpre; array[k][0] += tpre;
        array[l][1] = array[k][1] - tpim; array[k][1] += tpim;
      }
    }
  }
}

// Return sin/cos value angle in range 0.0 to 1.0 (0 is 0 degree, 1 is 360 degree)
void vna_sincosf(float angle, float * pSinVal, float * pCosVal) {
#ifndef __VNA_USE_MATH_TABLES__
  // Use default sin/cos functions
  angle *= 2 * VNA_PI;   // Convert to rad
  *pSinVal = sinf(angle);
  *pCosVal = cosf(angle);
#else
  float temp = angle - (int32_t)(angle); // Round to range -1.0f <= temp < 1.0f
  if (temp < 0.0f) temp+= 1.0f;          // Add 2*pi for negative value, 0 <= temp < 1.0f
  const uint32_t SIN_TABLE_SUB = SIN_TABLE_N - 2; // SIN table one sector N (full table must contain 4 sectors)
  // Get value index, sector and table index
  temp*= 1<<SIN_TABLE_N;                          // Get float table index
  const uint32_t table_index = temp;                    // Integer part for table index
  const float    frac = temp - table_index;             // Fractional part for interpolation
  const uint32_t sector = 1 << (table_index >> SIN_TABLE_SUB); // Get sector (0 to 3) mask: 0bXXXX for 3210
  const uint32_t sidx = table_index & ((1<<SIN_TABLE_SUB) - 1);// sin value index in table
  const uint32_t cidx = (1<<SIN_TABLE_SUB) - sidx;             // cos value index in table
  // Linear interpolate sin and cos values in table
  float s = SIN_TABLE[sidx] + frac * (SIN_TABLE[sidx + 1] - SIN_TABLE[sidx]);
  float c = SIN_TABLE[cidx] + frac * (SIN_TABLE[cidx - 1] - SIN_TABLE[cidx]);
  // Swap sin and cos for 1 and 3 sectors
  if (sector & 0b1010) SWAP(float, s, c);
  // Apply sin and cos digit for sectors
  // sector 0: +, +  | sector 1: -, +
  // sector 2: -, -  | sector 3: +, -
  if (sector & 0b0110) c = -c; // cos negative for 1 and 2 sectors
  if (sector & 0b1100) s = -s; // sin negative for 2 and 3 sectors
  *pSinVal = s;
  *pCosVal = c;
#endif
}

//**********************************************************************************
//      VNA math
//**********************************************************************************
// Cleanup declarations if used default math.h functions
#undef vna_sqrtf
#undef vna_cbrtf
#undef vna_logf
#undef vna_log10f_x_10
#undef vna_atanf
#undef vna_atan2f
#undef vna_atan2f_deg
#undef vna_modff

//**********************************************************************************
// modff function - return fractional part and integer from float value x
//**********************************************************************************
float vna_modff(float x, float *iptr) {
  union {float f; uint32_t i;} u = {x};
  int e = (int)((u.i>>23)&0xff) - 0x7f; // get exponent
  if (e <   0) {                        // no integral part
    if (iptr) *iptr = 0;
    return u.f;
  }
  if (e >= 23) x = 0;                   // no fractional part
  else {
    x = u.f; u.i&= ~(0x007fffff>>e);    // remove fractional part from u
    x-= u.f;                            // calc fractional part
  }
//if (iptr) *iptr = ((u.i&0x007fffff)|0x00800000)>>(23-e); // cut integer part from float as integer
  if (iptr) *iptr = u.f;                // cut integer part from float as float
  return x;
}
//**********************************************************************************
// square root
//**********************************************************************************
#if (__FPU_PRESENT == 0) && (__FPU_USED == 0)
#if 1
// __ieee754_sqrtf, remove some check (NAN, inf, normalization), small code optimization to arm
float vna_sqrtf(float x) {
  int32_t ix,s,q,m,t;
  uint32_t r;
  union {float f; uint32_t i;} u = {x};
  ix = u.i;
#if 0
  // take care of Inf and NaN
  if((ix&0x7f800000)==0x7f800000) return x*x+x;	// sqrt(NaN)=NaN, sqrt(+inf)=+inf, sqrt(-inf)=sNaN
  // take care if x < 0
  if (ix <  0) return (x-x)/0.0f;
#endif
  if (ix == 0) return 0.0f;
  m = (ix>>23);
#if 0 //
  // normalize x
  if(m==0) {				// subnormal x
    for(int i=0;(ix&0x00800000)==0;i++) ix<<=1;
      m -= i-1;
  }
#endif
  m -= 127;	// unbias exponent
  ix = (ix&0x007fffff)|0x00800000;
  // generate sqrt(x) bit by bit
  ix<<= (m&1) ? 2 : 1;	// odd m, double x to make it even, and after multiple by 2
  m >>= 1;	       		// m = [m/2]
  q = s = 0;			// q = sqrt(x)
  r = 0x01000000;		// r = moving bit from right to left
  while(r!=0) {
    t = s+r;
    if(t<=ix) {
      s    = t+r;
      ix  -= t;
      q   += r;
    }
    ix += ix;
    r>>=1;
  }
  // use floating add to find out rounding direction
  if(ix!=0) {
    if ((1.0f - 1e-30f) >= 1.0f) // trigger inexact flag.
      q += ((1.0f + 1e-30f) > 1.0f) ? 2 : (q&1);
  }
  ix = (q>>1)+0x3f000000;
  ix += (m <<23);
  u.i = ix;
  return u.f;
}
#else
// Simple implementation, but slow if no FPU used, and not usable if used hardware FPU sqrtf
float vna_sqrtf(float x) {
  union {float x; uint32_t i;} u = {x};
  u.i = (1<<29) + (u.i >> 1) - (1<<22);
  // Two Babylonian Steps (simplified from:)
  // u.x = 0.5f * (u.x + x/u.x);
  // u.x = 0.5f * (u.x + x/u.x);
  u.x =       u.x + x/u.x;
  u.x = 0.25f*u.x + x/u.x;

  return u.x;
}
#endif
#endif

//**********************************************************************************
// Cube root
//**********************************************************************************
float vna_cbrtf(float x) {
#if 1
  static const uint32_t
  B1 = 709958130, // B1 = (127-127.0/3-0.03306235651)*2**23
  B2 = 642849266; // B2 = (127-127.0/3-24/3-0.03306235651)*2**23

  float r,T;
  union {float f; uint32_t i;} u = {x};
  uint32_t hx = u.i & 0x7fffffff;

//	if (hx >= 0x7f800000)  // cbrt(NaN,INF) is itself
//		return x + x;
  // rough cbrtf to 5 bits
  if (hx < 0x00800000) {  // zero or subnormal?
    if (hx == 0)
      return x;  // cbrt(+-0) is itself
    u.f = x*0x1p24f;
    hx = u.i & 0x7fffffff;
    hx = hx/3 + B2;
  } else
    hx = hx/3 + B1;
  u.i &= 0x80000000;
  u.i |= hx;

  // First step Newton iteration (solving t*t-x/t == 0) to 16 bits.
  T = u.f;
  r = T*T*T;
  T*= (x+x+r)/(x+r+r);
// Second step Newton iteration to 47 bits.
  r = T*T*T;
  T*= (x+x+r)/(x+r+r);
  return T;
#else
  if (x == 0) {
    // would otherwise return something like 4.257959840008151e-109
    return 0;
  }
  float b = 1.0f; // use any value except 0
  float last_b_1 = 0;
  float last_b_2 = 0;
  while (last_b_1 != b && last_b_2 != b) {
    last_b_1 = b;
//    b = (b + x / (b * b)) / 2;
    b = (2 * b + x / b / b) / 3; // for small numbers, as suggested by  willywonka_dailyblah
    last_b_2 = b;
//    b = (b + x / (b * b)) / 2;
    b = (2 * b + x / b / b) / 3; //for small numbers, as suggested by  willywonka_dailyblah
  }
  return b;
#endif
}

//**********************************************************************************
// logf
//**********************************************************************************
float vna_logf(float x) {
  union {float f; int32_t i;} u = {x};
  if (u.i <= 0) return -1/0.0f;  // if <=0 return -inf
  const float MULT = logf(2.0f);
#if 0  // Give up to 0.0067  error
  const int log_2 = ((u.i >> 23) & 255) - 128;
  u.i = (u.i&0x007FFFFF) + 0x3F800000;
  u.f = ((-1.0f/3) * u.f + 2) * u.f - (2.0f/3);
  return (u.f + log_2) * MULT;
#elif 1 // Give up to 6.1e-5 error
  union { int32_t i; float f; } mx = { (u.i & 0x007FFFFF) | 0x3f000000 };
  return u.i * (MULT / (1 << 23)) - (124.225784301758f * MULT) - (1.497851252556f * MULT) * mx.f - (1.725635766983f * MULT) / (0.352076232433f + mx.f);
#else  // Give up to 4.768e-7 error
  float f, z, dk;
  static const float
    ln2_hi = 6.9313812256e-01,   // 0x3f317180
    ln2_lo = 9.0580006145e-06,   // 0x3717f7d1
    Lg0    = 2.0f,
    Lg1    = 0xaaaaaa.0p-24,     // 0.66666662693
    Lg2    = 0xccce13.0p-25,     // 0.40000972152
    Lg3    = 0x91e9ee.0p-25,     // 0.28498786688
    Lg4    = 0xf89e26.0p-26;     // 0.24279078841
  u.i += 0x3f800000 - 0x3f3504f3;// reduce x into [sqrt(2)/2, sqrt(2)]
  dk  = (int)(u.i>>23) - 0x7f;
  u.i = (u.i&0x007fffff) + 0x3f3504f3;
  f = (u.f - 1.0f)/(u.f + 1.0f);
  z = f*f;
  return f*(Lg0+z*(Lg1+z*(Lg2+z*(Lg3+z*Lg4)))) + dk*ln2_lo + dk*ln2_hi;
#endif
}

// Calculate 10.0 * log10f(x)
float vna_log10f_x_10(float x) {
  union {float f; int32_t i;} u = {x};
  if (u.i <= 0) return -1/0.0f;  // if <=0 return -inf
#if 0  // Give up to ~0.027  error
  const float MULT = (10.0f * logf(2.0f) / logf(10.0f));
  const int mx = (u.i >> 23) - 0x7f;
  u.i = (u.i&0x007FFFFF) + 0x3F800000;
  return (((-1.0f/3) * u.f + 2.0f) * u.f - (5.0f/3) + mx) * MULT;
#elif 1 // Give up to ~2.377e-4 error
  union { int32_t i; float f; } mx = { (u.i & 0x007FFFFF) | 0x3f000000 };
//  return u.i * (MULT / (1 << 23)) - (124.225784301758f * MULT) - (1.497851252556f * MULT) * mx.f - (1.725635766983f * MULT) / (0.352076232433f + mx.f);
  return u.i * 3.588558655063e-07 - 373.955116469345 - 4.509594876113 * mx.f - 5.197150890108 / (0.352256419296 + mx.f);
#else  // Give up to ~3.12e-6 error 0.0000038147f
  float f, z, mx;
  static const float             // Stored as float
    ln2_hi = 3.010264109333,     // 3.010264158249
    ln2_lo = 0.000035784926,     // 0.000035784928
    Lg0    = 8.685889638065,     // 20.0 / log(10.0)
    Lg1    = 2.895343684930,     // 2.895343780518
    Lg2    = 1.735529785085,     // 1.735529780388
    Lg3    = 1.257874186900,     // 1.257874131203
    Lg4    = 1.062503803849;     // 1.062503814697
  u.i += 0x3f800000 - 0x3f3504f3;// reduce x into [sqrt(2)/2, sqrt(2)]
  mx  = (u.i>>23) - 0x7f;
  u.i = (u.i&0x007fffff) + 0x3f3504f3;
  f = (u.f - 1.0f)/(u.f + 1.0f);
  z = f*f;
  return f*(Lg0+z*(Lg1+z*(Lg2+z*(Lg3+z*Lg4)))) + mx*ln2_lo + mx*ln2_hi;
#endif
}

//**********************************************************************************
// atanf
//**********************************************************************************
// __ieee754_atanf
float vna_atanf(float x) {
  static const float atanhi[] = {
    4.6364760399e-01, // atan(0.5)hi 0x3eed6338
    7.8539812565e-01, // atan(1.0)hi 0x3f490fda
    9.8279368877e-01, // atan(1.5)hi 0x3f7b985e
    1.5707962513e+00, // atan(inf)hi 0x3fc90fda
  };
  static const float atanlo[] = {
    5.0121582440e-09, // atan(0.5)lo 0x31ac3769
    3.7748947079e-08, // atan(1.0)lo 0x33222168
    3.4473217170e-08, // atan(1.5)lo 0x33140fb4
    7.5497894159e-08, // atan(inf)lo 0x33a22168
  };
  static const float aT[] = {
    3.3333328366e-01,
   -1.9999158382e-01,
    1.4253635705e-01,
   -1.0648017377e-01,
    6.1687607318e-02,
  };
  float w,s1,s2,z;
  uint32_t ix,sign;
  int id;
  union {float f; uint32_t i;} u = {x};
  ix = u.i;
  sign = ix>>31;
  ix &= 0x7fffffff;
  if (ix >= 0x4c800000) {  /* if |x| >= 2**26 */
    if (ix > 0x7f800000)
      return x;
    z = atanhi[3] + 0x1p-120f;
    return sign ? -z : z;
  }
  if (ix < 0x3ee00000) {   /* |x| < 0.4375 */
    if (ix < 0x39800000) {  /* |x| < 2**-12 */
      return x;
    }
    id = -1;
  } else {
    x = vna_fabsf(x);
    if (ix < 0x3f980000) {  /* |x| < 1.1875 */
      if (ix < 0x3f300000) {  /*  7/16 <= |x| < 11/16 */
        id = 0;
        x = (2.0f*x - 1.0f)/(2.0f + x);
      } else {                /* 11/16 <= |x| < 19/16 */
        id = 1;
        x = (x - 1.0f)/(x + 1.0f);
      }
    } else {
      if (ix < 0x401c0000) {  /* |x| < 2.4375 */
        id = 2;
        x = (x - 1.5f)/(1.0f + 1.5f*x);
      } else {                /* 2.4375 <= |x| < 2**26 */
        id = 3;
        x = -1.0f/x;
      }
    }
  }
  /* end of argument reduction */
  z = x*x;
  w = z*z;
  /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
  s1 = z*(aT[0]+w*(aT[2]+w*aT[4]));
  s2 = w*(aT[1]+w*aT[3]);
  if (id < 0)
    return x - x*(s1+s2);
  z = atanhi[id] - ((x*(s1+s2) - atanlo[id]) - x);
  return sign ? -z : z;
}

//**********************************************************************************
// atan2f
//**********************************************************************************
#if 0
// __ieee754_atan2f
float vna_atan2f(float y, float x) {
  static const float pi    = 3.1415927410e+00; // 0x40490fdb
  static const float pi_lo =-8.7422776573e-08; // 0xb3bbbd2e
  float z;
  uint32_t m,ix,iy;
  union {float f; uint32_t i;} ux = {x};
  union {float f; uint32_t i;} uy = {y};
  ix = ux.i;
  iy = uy.i;

  if (ix == 0x3f800000)  /* x=1.0 */
    return vna_atanf(y);
  m = ((iy>>31)&1) | ((ix>>30)&2);  /* 2*sign(x)+sign(y) */
  ix &= 0x7fffffff;
  iy &= 0x7fffffff;

  /* when y = 0 */
  if (iy == 0) {
    switch (m) {
      case 0:
      case 1: return   y; // atan(+-0,+anything)=+-0
      case 2: return  pi; // atan(+0,-anything) = pi
      case 3: return -pi; // atan(-0,-anything) =-pi
    }
  }
  /* when x = 0 */
  if (ix == 0)
    return m&1 ? -pi/2 : pi/2;
  /* when x is INF */
  if (ix == 0x7f800000) {
    if (iy == 0x7f800000) {
      switch (m) {
        case 0: return  pi/4; /* atan(+INF,+INF) */
        case 1: return -pi/4; /* atan(-INF,+INF) */
        case 2: return 3*pi/4;  /*atan(+INF,-INF)*/
        case 3: return -3*pi/4; /*atan(-INF,-INF)*/
      }
    } else {
      switch (m) {
        case 0: return  0.0f;    /* atan(+...,+INF) */
        case 1: return -0.0f;    /* atan(-...,+INF) */
        case 2: return  pi; /* atan(+...,-INF) */
        case 3: return -pi; /* atan(-...,-INF) */
      }
    }
  }
  /* |y/x| > 0x1p26 */
  if (ix+(26<<23) < iy || iy == 0x7f800000)
    return m&1 ? -pi/2 : pi/2;

  /* z = atan(|y/x|) with correct underflow */
  if ((m&2) && iy+(26<<23) < ix)  /*|y/x| < 0x1p-26, x < 0 */
    z = 0.0;
  else
    z = vna_atanf(vna_fabsf(y/x));
  switch (m) {
    case 0: return z;              /* atan(+,+) */
    case 1: return -z;             /* atan(-,+) */
    case 2: return pi - (z-pi_lo); /* atan(+,-) */
    default: /* case 3 */
      return (z-pi_lo) - pi; /* atan(-,-) */
  }
}

// Return atan(x, y) value in degree (-180 ... +180)
float vna_atan2f_deg(float y, float x) {
  return vna_atan2f(y, x) * (180.0f / VNA_PI));
}

#else
// Polynomial approximation to atan2f
float vna_atan2f (float y, float x) {
  union {float f; int32_t i;} ux = {x};
  union {float f; int32_t i;} uy = {y};
  if (ux.i == 0 && uy.i == 0)
    return 0.0f;

  float ax, ay, r, s;
  ax = vna_fabsf(x);
  ay = vna_fabsf(y);
  r = (ay < ax) ? ay / ax : ax / ay;
  s = r * r;
  // Polynomial approximation to atan(a) on [0,1]
#if 0
  // give 0.31 degree error
  r*= 0.970562748477141f - 0.189514164974601f * s;
  //r*= vna_fmaf(s, -0.189514164974601f, 0.970562748477141f);
#elif 0
  // give 0.04 degree error
  r*= 0.994949366116654f + s * (-0.287060635532652f + 0.078037176446441f * s);
  //r*= vna_fmaf(s, vna_fmaf(s, 0.078037176446441f, -0.287060635532652f), 0.994949366116654f);
#else
  // give 0.005 degree error
  r*= 0.999133448222780f + s * (-0.320533292381664f + s * (0.144982490144465f + s * -0.038254464970299f));
  //r*= vna_fmaf(s, vna_fmaf(s, vna_fmaf(s, -0.038254464970299f, 0.144982490144465f), -0.320533292381664f), 0.999133448222780f);
#endif
  // Map to full circle
  if (ay  > ax) r = VNA_PI/2.0f - r;
  if (ux.i < 0) r = VNA_PI      - r;
  if (uy.i < 0) r = -r;
  return r;
}

// Return atan(x, y) value in degree (-180 ... +180)
float vna_atan2f_deg(float y, float x) {
  union {float f; int32_t i;} ux = {x};
  union {float f; int32_t i;} uy = {y};
  if (ux.i == 0 && uy.i == 0)
    return 0.0f;

  float ax, ay, r, s;
  ax = vna_fabsf(x);
  ay = vna_fabsf(y);
  r = (ay < ax) ? ay / ax : ax / ay;
  s = r * r;
  // Polynomial approximation to atan(a)
#if 0  // max ~0.283729 degree error
  r*= 55.714078993f + s * -10.997807686f;
  // r*= vna_fmaf(s, -10.997807686f, 55.714078993f);
#elif 0  //max  ~0.034870 degree error
  r*= 57.029809913f + s * (-16.540732225f + s * 4.545792223f);
  // r*= vna_fmaf(s, vna_fmaf(s, 4.545792223f, -16.540732225f), 57.029809913f);
#elif 0 // max ~0.004662 degree error
  r*= 57.250736237f + s * (-18.401971817f + s * (8.380335808f + s * -2.233763933f));
  // r*= vna_fmaf(s, vna_fmaf(s, vna_fmaf(s, -2.233763933f, 8.380335808f),-18.401971817f), 57.250736237f);
#else  // max ~0.000655 degree error
  r*= 57.288120755f + s * (-18.925070157f + s * (10.322367203f + s * (-4.879099474f + s * 1.194337053f)));
  // r*= vna_fmaf(s, vna_fmaf(s, vna_fmaf(s, vna_fmaf(s, 1.194337053f, -4.879099474f), 10.322367203f), -18.925070157f), 57.288120755f);
#endif
  // Map to full circle (0-90, 90-180, etc.)
  if (ay > ax)  r =  90.0f - r;
  if (ux.i < 0) r = 180.0f - r;
  if (uy.i < 0) r = -r;
  return r;
}
#endif

//**********************************************************************************
// Fast expf approximation
//**********************************************************************************
float vna_expf(float x) {
  union { float f; int32_t i; } v;
  x*= (float)(1<<23) / logf(2.0f);
  v.i = (int32_t)x + 0x3F800000;
  int32_t m = (v.i >> 7) & 0xFFFF;  // copy mantissa
#if 0  // cubic spline approximation, empirical values for small maximum relative error (8.34e-5):
  v.i += ((((((((1277*m) >> 14) + 14825)*m) >> 14) - 79749)*m) >> 11) - 626;
#else  // quartic spline approximation, empirical values for small maximum relative error (1.21e-5):
  v.i += (((((((((((3537*m) >> 16) + 13668)*m) >> 18) + 15817)*m) >> 14) - 80470)*m) >> 11);
#endif
  return v.f;
}
