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
	 0.000000000f,  0.003067957f,  0.006135885f,  0.009203755f,  0.012271538f,  0.015339206f,  0.018406730f,  0.021474080f,
	 0.024541229f,  0.027608146f,  0.030674803f,  0.033741172f,  0.036807223f,  0.039872928f,  0.042938257f,  0.046003182f,
	 0.049067674f,  0.052131705f,  0.055195244f,  0.058258265f,  0.061320736f,  0.064382631f,  0.067443920f,  0.070504573f,
	 0.073564564f,  0.076623861f,  0.079682438f,  0.082740265f,  0.085797312f,  0.088853553f,  0.091908956f,  0.094963495f,
	 0.098017140f,  0.101069863f,  0.104121634f,  0.107172425f,  0.110222207f,  0.113270952f,  0.116318631f,  0.119365215f,
	 0.122410675f,  0.125454983f,  0.128498111f,  0.131540029f,  0.134580709f,  0.137620122f,  0.140658239f,  0.143695033f,
	 0.146730474f,  0.149764535f,  0.152797185f,  0.155828398f,  0.158858143f,  0.161886394f,  0.164913120f,  0.167938295f,
	 0.170961889f,  0.173983873f,  0.177004220f,  0.180022901f,  0.183039888f,  0.186055152f,  0.189068664f,  0.192080397f,
	 0.195090322f,  0.198098411f,  0.201104635f,  0.204108966f,  0.207111376f,  0.210111837f,  0.213110320f,  0.216106797f,
	 0.219101240f,  0.222093621f,  0.225083911f,  0.228072083f,  0.231058108f,  0.234041959f,  0.237023606f,  0.240003022f,
	 0.242980180f,  0.245955050f,  0.248927606f,  0.251897818f,  0.254865660f,  0.257831102f,  0.260794118f,  0.263754679f,
	 0.266712757f,  0.269668326f,  0.272621355f,  0.275571819f,  0.278519689f,  0.281464938f,  0.284407537f,  0.287347460f,
	 0.290284677f,  0.293219163f,  0.296150888f,  0.299079826f,  0.302005949f,  0.304929230f,  0.307849640f,  0.310767153f,
	 0.313681740f,  0.316593376f,  0.319502031f,  0.322407679f,  0.325310292f,  0.328209844f,  0.331106306f,  0.333999651f,
	 0.336889853f,  0.339776884f,  0.342660717f,  0.345541325f,  0.348418680f,  0.351292756f,  0.354163525f,  0.357030961f,
	 0.359895037f,  0.362755724f,  0.365612998f,  0.368466830f,  0.371317194f,  0.374164063f,  0.377007410f,  0.379847209f,
	 0.382683432f,  0.385516054f,  0.388345047f,  0.391170384f,  0.393992040f,  0.396809987f,  0.399624200f,  0.402434651f,
	 0.405241314f,  0.408044163f,  0.410843171f,  0.413638312f,  0.416429560f,  0.419216888f,  0.422000271f,  0.424779681f,
	 0.427555093f,  0.430326481f,  0.433093819f,  0.435857080f,  0.438616239f,  0.441371269f,  0.444122145f,  0.446868840f,
	 0.449611330f,  0.452349587f,  0.455083587f,  0.457813304f,  0.460538711f,  0.463259784f,  0.465976496f,  0.468688822f,
	 0.471396737f,  0.474100215f,  0.476799230f,  0.479493758f,  0.482183772f,  0.484869248f,  0.487550160f,  0.490226483f,
	 0.492898192f,  0.495565262f,  0.498227667f,  0.500885383f,  0.503538384f,  0.506186645f,  0.508830143f,  0.511468850f,
	 0.514102744f,  0.516731799f,  0.519355990f,  0.521975293f,  0.524589683f,  0.527199135f,  0.529803625f,  0.532403128f,
	 0.534997620f,  0.537587076f,  0.540171473f,  0.542750785f,  0.545324988f,  0.547894059f,  0.550457973f,  0.553016706f,
	 0.555570233f,  0.558118531f,  0.560661576f,  0.563199344f,  0.565731811f,  0.568258953f,  0.570780746f,  0.573297167f,
	 0.575808191f,  0.578313796f,  0.580813958f,  0.583308653f,  0.585797857f,  0.588281548f,  0.590759702f,  0.593232295f,
	 0.595699304f,  0.598160707f,  0.600616479f,  0.603066599f,  0.605511041f,  0.607949785f,  0.610382806f,  0.612810082f,
	 0.615231591f,  0.617647308f,  0.620057212f,  0.622461279f,  0.624859488f,  0.627251815f,  0.629638239f,  0.632018736f,
	 0.634393284f,  0.636761861f,  0.639124445f,  0.641481013f,  0.643831543f,  0.646176013f,  0.648514401f,  0.650846685f,
	 0.653172843f,  0.655492853f,  0.657806693f,  0.660114342f,  0.662415778f,  0.664710978f,  0.666999922f,  0.669282588f,
	 0.671558955f,  0.673829000f,  0.676092704f,  0.678350043f,  0.680600998f,  0.682845546f,  0.685083668f,  0.687315341f,
	 0.689540545f,  0.691759258f,  0.693971461f,  0.696177131f,  0.698376249f,  0.700568794f,  0.702754744f,  0.704934080f,
	 0.707106781f,  0.709272826f,  0.711432196f,  0.713584869f,  0.715730825f,  0.717870045f,  0.720002508f,  0.722128194f,
	 0.724247083f,  0.726359155f,  0.728464390f,  0.730562769f,  0.732654272f,  0.734738878f,  0.736816569f,  0.738887324f,
	 0.740951125f,  0.743007952f,  0.745057785f,  0.747100606f,  0.749136395f,  0.751165132f,  0.753186799f,  0.755201377f,
	 0.757208847f,  0.759209189f,  0.761202385f,  0.763188417f,  0.765167266f,  0.767138912f,  0.769103338f,  0.771060524f,
	 0.773010453f,  0.774953107f,  0.776888466f,  0.778816512f,  0.780737229f,  0.782650596f,  0.784556597f,  0.786455214f,
	 0.788346428f,  0.790230221f,  0.792106577f,  0.793975478f,  0.795836905f,  0.797690841f,  0.799537269f,  0.801376172f,
	 0.803207531f,  0.805031331f,  0.806847554f,  0.808656182f,  0.810457198f,  0.812250587f,  0.814036330f,  0.815814411f,
	 0.817584813f,  0.819347520f,  0.821102515f,  0.822849781f,  0.824589303f,  0.826321063f,  0.828045045f,  0.829761234f,
	 0.831469612f,  0.833170165f,  0.834862875f,  0.836547727f,  0.838224706f,  0.839893794f,  0.841554977f,  0.843208240f,
	 0.844853565f,  0.846490939f,  0.848120345f,  0.849741768f,  0.851355193f,  0.852960605f,  0.854557988f,  0.856147328f,
	 0.857728610f,  0.859301818f,  0.860866939f,  0.862423956f,  0.863972856f,  0.865513624f,  0.867046246f,  0.868570706f,
	 0.870086991f,  0.871595087f,  0.873094978f,  0.874586652f,  0.876070094f,  0.877545290f,  0.879012226f,  0.880470889f,
	 0.881921264f,  0.883363339f,  0.884797098f,  0.886222530f,  0.887639620f,  0.889048356f,  0.890448723f,  0.891840709f,
	 0.893224301f,  0.894599486f,  0.895966250f,  0.897324581f,  0.898674466f,  0.900015892f,  0.901348847f,  0.902673318f,
	 0.903989293f,  0.905296759f,  0.906595705f,  0.907886116f,  0.909167983f,  0.910441292f,  0.911706032f,  0.912962190f,
	 0.914209756f,  0.915448716f,  0.916679060f,  0.917900776f,  0.919113852f,  0.920318277f,  0.921514039f,  0.922701128f,
	 0.923879533f,  0.925049241f,  0.926210242f,  0.927362526f,  0.928506080f,  0.929640896f,  0.930766961f,  0.931884266f,
	 0.932992799f,  0.934092550f,  0.935183510f,  0.936265667f,  0.937339012f,  0.938403534f,  0.939459224f,  0.940506071f,
	 0.941544065f,  0.942573198f,  0.943593458f,  0.944604837f,  0.945607325f,  0.946600913f,  0.947585591f,  0.948561350f,
	 0.949528181f,  0.950486074f,  0.951435021f,  0.952375013f,  0.953306040f,  0.954228095f,  0.955141168f,  0.956045251f,
	 0.956940336f,  0.957826413f,  0.958703475f,  0.959571513f,  0.960430519f,  0.961280486f,  0.962121404f,  0.962953267f,
	 0.963776066f,  0.964589793f,  0.965394442f,  0.966190003f,  0.966976471f,  0.967753837f,  0.968522094f,  0.969281235f,
	 0.970031253f,  0.970772141f,  0.971503891f,  0.972226497f,  0.972939952f,  0.973644250f,  0.974339383f,  0.975025345f,
	 0.975702130f,  0.976369731f,  0.977028143f,  0.977677358f,  0.978317371f,  0.978948175f,  0.979569766f,  0.980182136f,
	 0.980785280f,  0.981379193f,  0.981963869f,  0.982539302f,  0.983105487f,  0.983662419f,  0.984210092f,  0.984748502f,
	 0.985277642f,  0.985797509f,  0.986308097f,  0.986809402f,  0.987301418f,  0.987784142f,  0.988257568f,  0.988721692f,
	 0.989176510f,  0.989622017f,  0.990058210f,  0.990485084f,  0.990902635f,  0.991310860f,  0.991709754f,  0.992099313f,
	 0.992479535f,  0.992850414f,  0.993211949f,  0.993564136f,  0.993906970f,  0.994240449f,  0.994564571f,  0.994879331f,
	 0.995184727f,  0.995480755f,  0.995767414f,  0.996044701f,  0.996312612f,  0.996571146f,  0.996820299f,  0.997060070f,
	 0.997290457f,  0.997511456f,  0.997723067f,  0.997925286f,  0.998118113f,  0.998301545f,  0.998475581f,  0.998640218f,
	 0.998795456f,  0.998941293f,  0.999077728f,  0.999204759f,  0.999322385f,  0.999430605f,  0.999529418f,  0.999618822f,
	 0.999698819f,  0.999769405f,  0.999830582f,  0.999882347f,  0.999924702f,  0.999957645f,  0.999981175f,  0.999995294f,
	 1.000000000f
};

const float sin_table_1024[1024/4 + 1] = {
	 0.000000000f,  0.006135885f,  0.012271538f,  0.018406730f,  0.024541229f,  0.030674803f,  0.036807223f,  0.042938257f,
	 0.049067674f,  0.055195244f,  0.061320736f,  0.067443920f,  0.073564564f,  0.079682438f,  0.085797312f,  0.091908956f,
	 0.098017140f,  0.104121634f,  0.110222207f,  0.116318631f,  0.122410675f,  0.128498111f,  0.134580709f,  0.140658239f,
	 0.146730474f,  0.152797185f,  0.158858143f,  0.164913120f,  0.170961889f,  0.177004220f,  0.183039888f,  0.189068664f,
	 0.195090322f,  0.201104635f,  0.207111376f,  0.213110320f,  0.219101240f,  0.225083911f,  0.231058108f,  0.237023606f,
	 0.242980180f,  0.248927606f,  0.254865660f,  0.260794118f,  0.266712757f,  0.272621355f,  0.278519689f,  0.284407537f,
	 0.290284677f,  0.296150888f,  0.302005949f,  0.307849640f,  0.313681740f,  0.319502031f,  0.325310292f,  0.331106306f,
	 0.336889853f,  0.342660717f,  0.348418680f,  0.354163525f,  0.359895037f,  0.365612998f,  0.371317194f,  0.377007410f,
	 0.382683432f,  0.388345047f,  0.393992040f,  0.399624200f,  0.405241314f,  0.410843171f,  0.416429560f,  0.422000271f,
	 0.427555093f,  0.433093819f,  0.438616239f,  0.444122145f,  0.449611330f,  0.455083587f,  0.460538711f,  0.465976496f,
	 0.471396737f,  0.476799230f,  0.482183772f,  0.487550160f,  0.492898192f,  0.498227667f,  0.503538384f,  0.508830143f,
	 0.514102744f,  0.519355990f,  0.524589683f,  0.529803625f,  0.534997620f,  0.540171473f,  0.545324988f,  0.550457973f,
	 0.555570233f,  0.560661576f,  0.565731811f,  0.570780746f,  0.575808191f,  0.580813958f,  0.585797857f,  0.590759702f,
	 0.595699304f,  0.600616479f,  0.605511041f,  0.610382806f,  0.615231591f,  0.620057212f,  0.624859488f,  0.629638239f,
	 0.634393284f,  0.639124445f,  0.643831543f,  0.648514401f,  0.653172843f,  0.657806693f,  0.662415778f,  0.666999922f,
	 0.671558955f,  0.676092704f,  0.680600998f,  0.685083668f,  0.689540545f,  0.693971461f,  0.698376249f,  0.702754744f,
	 0.707106781f,  0.711432196f,  0.715730825f,  0.720002508f,  0.724247083f,  0.728464390f,  0.732654272f,  0.736816569f,
	 0.740951125f,  0.745057785f,  0.749136395f,  0.753186799f,  0.757208847f,  0.761202385f,  0.765167266f,  0.769103338f,
	 0.773010453f,  0.776888466f,  0.780737229f,  0.784556597f,  0.788346428f,  0.792106577f,  0.795836905f,  0.799537269f,
	 0.803207531f,  0.806847554f,  0.810457198f,  0.814036330f,  0.817584813f,  0.821102515f,  0.824589303f,  0.828045045f,
	 0.831469612f,  0.834862875f,  0.838224706f,  0.841554977f,  0.844853565f,  0.848120345f,  0.851355193f,  0.854557988f,
	 0.857728610f,  0.860866939f,  0.863972856f,  0.867046246f,  0.870086991f,  0.873094978f,  0.876070094f,  0.879012226f,
	 0.881921264f,  0.884797098f,  0.887639620f,  0.890448723f,  0.893224301f,  0.895966250f,  0.898674466f,  0.901348847f,
	 0.903989293f,  0.906595705f,  0.909167983f,  0.911706032f,  0.914209756f,  0.916679060f,  0.919113852f,  0.921514039f,
	 0.923879533f,  0.926210242f,  0.928506080f,  0.930766961f,  0.932992799f,  0.935183510f,  0.937339012f,  0.939459224f,
	 0.941544065f,  0.943593458f,  0.945607325f,  0.947585591f,  0.949528181f,  0.951435021f,  0.953306040f,  0.955141168f,
	 0.956940336f,  0.958703475f,  0.960430519f,  0.962121404f,  0.963776066f,  0.965394442f,  0.966976471f,  0.968522094f,
	 0.970031253f,  0.971503891f,  0.972939952f,  0.974339383f,  0.975702130f,  0.977028143f,  0.978317371f,  0.979569766f,
	 0.980785280f,  0.981963869f,  0.983105487f,  0.984210092f,  0.985277642f,  0.986308097f,  0.987301418f,  0.988257568f,
	 0.989176510f,  0.990058210f,  0.990902635f,  0.991709754f,  0.992479535f,  0.993211949f,  0.993906970f,  0.994564571f,
	 0.995184727f,  0.995767414f,  0.996312612f,  0.996820299f,  0.997290457f,  0.997723067f,  0.998118113f,  0.998475581f,
	 0.998795456f,  0.999077728f,  0.999322385f,  0.999529418f,  0.999698819f,  0.999830582f,  0.999924702f,  0.999981175f,
	 1.000000000f
};

const float sin_table_512[512/4 + 1] = {
	 0.000000000f,  0.012271538f,  0.024541229f,  0.036807223f,  0.049067674f,  0.061320736f,  0.073564564f,  0.085797312f,
	 0.098017140f,  0.110222207f,  0.122410675f,  0.134580709f,  0.146730474f,  0.158858143f,  0.170961889f,  0.183039888f,
	 0.195090322f,  0.207111376f,  0.219101240f,  0.231058108f,  0.242980180f,  0.254865660f,  0.266712757f,  0.278519689f,
	 0.290284677f,  0.302005949f,  0.313681740f,  0.325310292f,  0.336889853f,  0.348418680f,  0.359895037f,  0.371317194f,
	 0.382683432f,  0.393992040f,  0.405241314f,  0.416429560f,  0.427555093f,  0.438616239f,  0.449611330f,  0.460538711f,
	 0.471396737f,  0.482183772f,  0.492898192f,  0.503538384f,  0.514102744f,  0.524589683f,  0.534997620f,  0.545324988f,
	 0.555570233f,  0.565731811f,  0.575808191f,  0.585797857f,  0.595699304f,  0.605511041f,  0.615231591f,  0.624859488f,
	 0.634393284f,  0.643831543f,  0.653172843f,  0.662415778f,  0.671558955f,  0.680600998f,  0.689540545f,  0.698376249f,
	 0.707106781f,  0.715730825f,  0.724247083f,  0.732654272f,  0.740951125f,  0.749136395f,  0.757208847f,  0.765167266f,
	 0.773010453f,  0.780737229f,  0.788346428f,  0.795836905f,  0.803207531f,  0.810457198f,  0.817584813f,  0.824589303f,
	 0.831469612f,  0.838224706f,  0.844853565f,  0.851355193f,  0.857728610f,  0.863972856f,  0.870086991f,  0.876070094f,
	 0.881921264f,  0.887639620f,  0.893224301f,  0.898674466f,  0.903989293f,  0.909167983f,  0.914209756f,  0.919113852f,
	 0.923879533f,  0.928506080f,  0.932992799f,  0.937339012f,  0.941544065f,  0.945607325f,  0.949528181f,  0.953306040f,
	 0.956940336f,  0.960430519f,  0.963776066f,  0.966976471f,  0.970031253f,  0.972939952f,  0.975702130f,  0.978317371f,
	 0.980785280f,  0.983105487f,  0.985277642f,  0.987301418f,  0.989176510f,  0.990902635f,  0.992479535f,  0.993906970f,
	 0.995184727f,  0.996312612f,  0.997290457f,  0.998118113f,  0.998795456f,  0.999322385f,  0.999698819f,  0.999924702f,
	 1.000000000f
};

const float sin_table_256[256/4 + 1] = {
	 0.000000000f,  0.024541229f,  0.049067674f,  0.073564564f,  0.098017140f,  0.122410675f,  0.146730474f,  0.170961889f,
	 0.195090322f,  0.219101240f,  0.242980180f,  0.266712757f,  0.290284677f,  0.313681740f,  0.336889853f,  0.359895037f,
	 0.382683432f,  0.405241314f,  0.427555093f,  0.449611330f,  0.471396737f,  0.492898192f,  0.514102744f,  0.534997620f,
	 0.555570233f,  0.575808191f,  0.595699304f,  0.615231591f,  0.634393284f,  0.653172843f,  0.671558955f,  0.689540545f,
	 0.707106781f,  0.724247083f,  0.740951125f,  0.757208847f,  0.773010453f,  0.788346428f,  0.803207531f,  0.817584813f,
	 0.831469612f,  0.844853565f,  0.857728610f,  0.870086991f,  0.881921264f,  0.893224301f,  0.903989293f,  0.914209756f,
	 0.923879533f,  0.932992799f,  0.941544065f,  0.949528181f,  0.956940336f,  0.963776066f,  0.970031253f,  0.975702130f,
	 0.980785280f,  0.985277642f,  0.989176510f,  0.992479535f,  0.995184727f,  0.997290457f,  0.998795456f,  0.999698819f,
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
#elif 0 // Use shifts
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
#if 0
        float tpre = array[l][0] * c - array[l][1] * s;
        float tpim = array[l][0] * s + array[l][1] * c;
        array[l][0] = array[k][0] - tpre; array[k][0] += tpre;
        array[l][1] = array[k][1] - tpim; array[k][1] += tpim;
#else
        float tpre = vna_fmaf(array[l][0], c,-array[l][1] * s);
        float tpim = vna_fmaf(array[l][0], s, array[l][1] * c);
        array[l][0] = array[k][0] - tpre; array[k][0] += tpre;
        array[l][1] = array[k][1] - tpim; array[k][1] += tpim;
#endif
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
  union {float f; int32_t i;} ux = {x}; x = vna_fabsf(x);
  union {float f; int32_t i;} uy = {y}; y = vna_fabsf(y);
  if (x == 0.0f && y == 0.0f)
    return 0.0f;

  float r = (y > x) ? x / y : y / x;
  float s = r * r;
  // Polynomial approximation to atan(a) on [0,1]
#if 0   // give 0.31 degree error
  r*= 0.970562748477141f - 0.189514164974601f * s;
  //r*= vna_fmaf(s, -0.189514164974601f, 0.970562748477141f);
#elif 0 // give 0.04 degree error
  r*= 0.994949366116654f + s * (-0.287060635532652f + 0.078037176446441f * s);
  //r*= vna_fmaf(s, vna_fmaf(s, 0.078037176446441f, -0.287060635532652f), 0.994949366116654f);
#else   // give 0.005 degree error
  r*= 0.999133448222780f + s * (-0.320533292381664f + s * (0.144982490144465f + s * -0.038254464970299f));
  //r*= vna_fmaf(s, vna_fmaf(s, vna_fmaf(s, -0.038254464970299f, 0.144982490144465f), -0.320533292381664f), 0.999133448222780f);
#endif
  // Map to full circle
  if (  y  > x) r = VNA_PI/2.0f - r;
  if (ux.i < 0) r = VNA_PI      - r;
  if (uy.i < 0) r = -r;
  return r;
}

// Return atan(x, y) value in degree (-180 ... +180)
float vna_atan2f_deg(float y, float x) {
  union {float f; int32_t i;} ux = {x}; x = vna_fabsf(x);
  union {float f; int32_t i;} uy = {y}; y = vna_fabsf(y);
  if (x == 0.0f && y == 0.0f)
    return 0.0f;

  float r = (y > x) ? x / y : y / x;
  float s = r * r;
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
  if (   y > x) r =  90.0f - r;
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
#else  // quartic spline approximation, empirical values for small maximum relative error (1e-5):
  v.i += (((((((((((903*m) >> 14) + 13521)*m) >> 18) + 15838)*m) >> 14) - 80482)*m) >> 11);
#endif
  return v.f;
}
