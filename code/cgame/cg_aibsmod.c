#include "cg_local.h"

//used to initialize min/max values
#define IMPOSSIBLE_SPEED (1000000.0f)

#define MAX_SPEEDCALC_FRAMES 256

float getPlayerSpeed(void)
{
	static float previousSpeeds[MAX_SPEEDCALC_FRAMES];
	static float previousHeights[MAX_SPEEDCALC_FRAMES];
	static int index;
	int totalFrames;
	float total;
	float maxval; //max. height or speed, depending on method
	int i;

	if (am_drawSpeedFull.integer)
		previousSpeeds[index % MAX_SPEEDCALC_FRAMES] = cg.xyzspeed;
	else
		previousSpeeds[index % MAX_SPEEDCALC_FRAMES] = cg.xyspeed;

	previousHeights[index % MAX_SPEEDCALC_FRAMES] = cg.zpos;
	index = (index + 1) % MAX_SPEEDCALC_FRAMES;

	totalFrames = am_drawSpeedFrames.integer;

	if (totalFrames <= 0)
		totalFrames = 1;
	else if (totalFrames >= MAX_SPEEDCALC_FRAMES)
		totalFrames = MAX_SPEEDCALC_FRAMES-1;

	if (am_drawSpeedMethod.integer == 2) { //speed at max. height
		total = 0.0f;
		maxval = -IMPOSSIBLE_SPEED;
		for (i=(index + MAX_SPEEDCALC_FRAMES - totalFrames)%MAX_SPEEDCALC_FRAMES; i!=index; i=(i+1)%MAX_SPEEDCALC_FRAMES) {
			if (previousHeights[i] > maxval) {
				total = previousSpeeds[i];
				maxval = previousHeights[i];
			}
		}
	}

	else if (am_drawSpeedMethod.integer == 1) { //speed at max. speed
		total = 0.0f;
		maxval = -1.0f;
		for (i=(index + MAX_SPEEDCALC_FRAMES - totalFrames)%MAX_SPEEDCALC_FRAMES; i!=index; i=(i+1)%MAX_SPEEDCALC_FRAMES) {
			if (previousSpeeds[i] > maxval) {
				total = previousSpeeds[i];
				maxval = previousSpeeds[i];
			}
		}
	}

	else if (am_drawSpeedMethod.integer == 3) { //speed at min. speed
		total = 0.0f;
		maxval = +IMPOSSIBLE_SPEED;
		for (i=(index + MAX_SPEEDCALC_FRAMES - totalFrames)%MAX_SPEEDCALC_FRAMES; i!=index; i=(i+1)%MAX_SPEEDCALC_FRAMES) {
			if (previousSpeeds[i] < maxval) {
				total = previousSpeeds[i];
				maxval = previousSpeeds[i];
			}
		}
	}

	else { //average speed
		maxval = 0.0f;
		for (i=(index + MAX_SPEEDCALC_FRAMES - totalFrames)%MAX_SPEEDCALC_FRAMES; i!=index; i=(i+1)%MAX_SPEEDCALC_FRAMES) {
			total += previousSpeeds[i];
			maxval += 1.0f;
		}
		total /= maxval;
	}

	return total;
}

float CG_DrawBigSpeed(float y)
{
	char *s;
	int w;

	s = va("%.0fups", getPlayerSpeed());
	w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;

	CG_DrawBigString(635 - w, y + 2, s, 1.0f);

	return y + BIGCHAR_HEIGHT + 4;
}

void CG_DrawSmallSpeed(void)
{
	static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	char *s;
	int w;

	s = va("%.0f", getPlayerSpeed());

	w = CG_DrawStrlen(s) * TINYCHAR_WIDTH;

	CG_DrawStringExt(320 - (w/2), 304, s, color, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0);
}

void CG_DrawButtons(void)
{
	if (cg.buttonState & BTNFLAG_BUTTON_UP)
		CG_DrawPic(304, 320+1, 32, 32, cgs.media.button_up_downShader);
	else
		CG_DrawPic(304, 320+1, 32, 32, cgs.media.button_up_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_LEFT)
		CG_DrawPic(272+1, 352, 32, 32, cgs.media.button_left_downShader);
	else
		CG_DrawPic(272+1, 352, 32, 32, cgs.media.button_left_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_RIGHT)
		CG_DrawPic(336-1, 352, 32, 32, cgs.media.button_right_downShader);
	else
		CG_DrawPic(336-1, 352, 32, 32, cgs.media.button_right_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_DOWN)
		CG_DrawPic(304, 352, 32, 32, cgs.media.button_down_downShader);
	else
		CG_DrawPic(304, 352, 32, 32, cgs.media.button_down_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_JUMP)
		CG_DrawPic(240, 400, 160, 32, cgs.media.button_jump_downShader);
	else
		CG_DrawPic(240, 400, 160, 32, cgs.media.button_jump_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_FIRE)
		CG_DrawPic(240, 320, 32, 32, cgs.media.button_fire_downShader);
	else
		CG_DrawPic(240, 320, 32, 32, cgs.media.button_fire_upShader);
}

void CG_AibsmodEntities()
{
//	if (cgs.gametype == GT_FOOTBALL && am_drawFootballTracer.integer)
//		CG_DrawFootballTracer();
}

//Draw a laser beam
void CG_Laser(const vec3_t start, const vec3_t end, const vec3_t color)
{
	refEntity_t ent;

	memset(&ent, 0, sizeof(ent));

	ent.reType = RT_RAIL_CORE;
	ent.customShader = cgs.media.railCoreShader;

	VectorCopy(start, ent.origin);
	VectorCopy(end, ent.oldorigin);

	ent.shaderRGBA[0] = color[0] * 255;
	ent.shaderRGBA[1] = color[1] * 255;
	ent.shaderRGBA[2] = color[2] * 255;
	ent.shaderRGBA[3] = 255;

	trap_R_AddRefEntityToScene(&ent);
}

//draw the football
void CG_Football(centity_t *cent)
{
	refEntity_t ent;

	memset(&ent, 0, sizeof(ent));

	//set frame
	ent.frame = cent->currentState.frame;
	ent.oldframe = ent.frame;
	ent.backlerp = 0;

	VectorCopy(cent->lerpOrigin, ent.origin);
	VectorCopy(cent->lerpOrigin, ent.oldorigin);

	//copy position to global variable
	VectorCopy(cent->lerpOrigin, cg.footballPos);

	ent.hModel = cgs.media.footballModel;

	//convert angles to axis
	AnglesToAxis(cent->lerpAngles, ent.axis);

	//add to refresh list
	trap_R_AddRefEntityToScene(&ent);
}

//draw goal posts
void CG_FootballGoalpost(centity_t *cent)
{
	refEntity_t ent;

	memset(&ent, 0, sizeof(ent));

	//set frame
	ent.frame = cent->currentState.frame;
	ent.oldframe = ent.frame;
	ent.backlerp = 0;

	VectorCopy(cent->lerpOrigin, ent.origin);
	VectorCopy(cent->lerpOrigin, ent.oldorigin);

	switch (cent->currentState.modelindex) {
		case 11: //red top
			ent.hModel = cgs.media.goalpostRedTopModel;
			break;

		case 12: //red back
			ent.hModel = cgs.media.goalpostRedBackModel;
			break;

		case 13: //red left
			ent.hModel = cgs.media.goalpostRedLeftModel;
			break;

		case 14: //red right
			ent.hModel = cgs.media.goalpostRedRightModel;
			break;

		case 21: //blue top
			ent.hModel = cgs.media.goalpostBlueTopModel;
			break;

		case 22: //blue back
			ent.hModel = cgs.media.goalpostBlueBackModel;
			break;

		case 23: //blue left
			ent.hModel = cgs.media.goalpostBlueLeftModel;
			break;

		case 24: //blue right
			ent.hModel = cgs.media.goalpostBlueRightModel;
			break;

		default:
			return;
	}

	//convert angles to axis
	AnglesToAxis(cent->lerpAngles, ent.axis);

	//add to refresh list
	trap_R_AddRefEntityToScene(&ent);
}

//draw a laser tripmine
void CG_Tripmine(centity_t *cent)
{
	refEntity_t	mineBody;
	vec3_t		laserColor;
	int			timePart;

	memset(&mineBody, 0, sizeof(mineBody));

	//set frame
	mineBody.frame = cent->currentState.frame;
	mineBody.oldframe = mineBody.frame;
	mineBody.backlerp = 0;

	VectorCopy(cent->lerpOrigin, mineBody.origin);
	VectorCopy(cent->lerpOrigin, mineBody.oldorigin);

	mineBody.hModel = cgs.media.tripmineModel;

	CG_SetShaderColors(cent->currentState.otherEntityNum, AM_COLORPART_1, mineBody.shaderRGBA);
	mineBody.shaderRGBA[3] = 255;

	//convert angles to axis
	AnglesToAxis(cent->lerpAngles, mineBody.axis);

	//add to refresh list
	trap_R_AddRefEntityToScene(&mineBody);

	timePart = (cg.time + cent->currentState.generic1) % 2000; //add current + creation time and get flicker phase

	//Draw laser if armed
	if (cent->currentState.modelindex == 1) { //armed
		if (timePart < 50) //flicker for 50 ms. every 2000 ms.
			CG_SetColors(cent->currentState.otherEntityNum, AM_COLORPART_2, laserColor);
		else
			CG_SetColors(cent->currentState.otherEntityNum, AM_COLORPART_1, laserColor);

		CG_Laser(cent->currentState.origin, cent->currentState.angles2, laserColor);
	}
}

//draw a bounding box (mins=origin2 and maxs=angles2)
void CG_BBox(centity_t *cent)
{
	clientInfo_t	ci;
	vec3_t			mins, maxs;
	vec3_t			p1, p2;
	const vec3_t	color = { 0.5f, 0.0f, 1.0f };

	VectorAdd(cent->currentState.origin, cent->currentState.origin2, mins);
	VectorAdd(cent->currentState.origin, cent->currentState.angles2, maxs);

	VectorSet(p1, mins[0], mins[1], mins[2]);
	VectorSet(p2, mins[0], maxs[1], mins[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, maxs[0], maxs[1], mins[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p2, maxs[0], mins[1], mins[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, mins[0], mins[1], mins[2]);
	CG_Laser(p1, p2, color);

	VectorSet(p1, mins[0], mins[1], maxs[2]);
	VectorSet(p2, mins[0], maxs[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, maxs[0], maxs[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p2, maxs[0], mins[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, mins[0], mins[1], maxs[2]);
	CG_Laser(p1, p2, color);

	VectorSet(p1, mins[0], mins[1], mins[2]);
	VectorSet(p2, mins[0], mins[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, mins[0], maxs[1], mins[2]);
	VectorSet(p2, mins[0], maxs[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, maxs[0], mins[1], mins[2]);
	VectorSet(p2, maxs[0], mins[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, maxs[0], maxs[1], mins[2]);
	VectorSet(p2, maxs[0], maxs[1], maxs[2]);
	CG_Laser(p1, p2, color);
}

//Draw a tracer line from player to football
void CG_DrawFootballTracer(void)
{
	vec3_t tracerColor;

	if (cg.carrierNum == -1) //noone is carrying the ball
		VectorSet(tracerColor, 8.0f, 8.0f, 8.0f);
	else if (cgs.clientinfo[cg.carrierNum].team == TEAM_RED)
		VectorSet(tracerColor, 1.0f, 0.15f, 0.15f);
	else if (cgs.clientinfo[cg.carrierNum].team == TEAM_BLUE)
		VectorSet(tracerColor, 0.15f, 0.15f, 1.0f);
	else
		VectorSet(tracerColor, 0.15f, 1.0f, 0.15f);

	CG_Laser(cg.predictedPlayerState.origin, cg.footballPos, tracerColor);
}

//Called after dropping weapon
void CG_DropWeaponChange(void)
{
	int i;
	int original;

	original = cg.weaponSelect;
	cg.weaponSelectTime = cg.time;

	for (i=15; i>0; i--) {
		if (i == original)
			continue;

		if (CG_WeaponSelectable(i)) {
			cg.weaponSelect = i;
			break;
		}
	}
}

//CG_SetColors below uses this to color body parts
int CG_SetColorPart(char *colorString, amColorpart_t part, float targetRGB[3])
{
	switch (colorString[part]) {
		case '1': //blue
			targetRGB[0] = 0.0f;
			targetRGB[1] = 0.0f;
			targetRGB[2] = 1.0f;
			return 1;

		case '2': //green
			targetRGB[0] = 0.0f;
			targetRGB[1] = 1.0f;
			targetRGB[2] = 0.0f;
			return 1;

		case '3': //cyan
			targetRGB[0] = 0.0f;
			targetRGB[1] = 1.0f;
			targetRGB[2] = 1.0f;
			return 1;

		case '4': //red
			targetRGB[0] = 1.0f;
			targetRGB[1] = 0.0f;
			targetRGB[2] = 0.0f;
			return 1;

		case '5': //magenta
			targetRGB[0] = 1.0f;
			targetRGB[1] = 0.0f;
			targetRGB[2] = 1.0f;
			return 1;

		case '6': //yellow
			targetRGB[0] = 1.0f;
			targetRGB[1] = 1.0f;
			targetRGB[2] = 0.0f;
			return 1;

		case '7': //white
			targetRGB[0] = 1.0f;
			targetRGB[1] = 1.0f;
			targetRGB[2] = 1.0f;
			return 1;

		case '8': //gray
			targetRGB[0] = 0.5f;
			targetRGB[1] = 0.5f;
			targetRGB[2] = 0.5f;
			return 1;

		case 'a':
			targetRGB[0] = 0.5f;
			targetRGB[1] = 0.5f;
			targetRGB[2] = 1.0f;
			return 1;

		case 'b':
			targetRGB[0] = 0.5f;
			targetRGB[1] = 1.0f;
			targetRGB[2] = 0.5f;
			return 1;

		case 'c':
			targetRGB[0] = 1.0f;
			targetRGB[1] = 0.5f;
			targetRGB[2] = 0.5f;
			return 1;

		case 'd':
			targetRGB[0] = 0.5f;
			targetRGB[1] = 0.0f;
			targetRGB[2] = 1.0f;
			return 1;

		case 'e':
			targetRGB[0] = 0.0f;
			targetRGB[1] = 0.5f;
			targetRGB[2] = 1.0f;
			return 1;

		case 'f':
			targetRGB[0] = 0.5f;
			targetRGB[1] = 1.0f;
			targetRGB[2] = 0.0f;
			return 1;

		case 'g':
			targetRGB[0] = 0.0f;
			targetRGB[1] = 1.0f;
			targetRGB[2] = 0.5f;
			return 1;

		case 'h':
			targetRGB[0] = 1.0f;
			targetRGB[1] = 0.5f;
			targetRGB[2] = 0.0f;
			return 1;

		case 'i': //pink
			targetRGB[0] = 1.0f;
			targetRGB[1] = 0.0f;
			targetRGB[2] = 0.5f;
			return 1;

		default:
			return 0;
	}
}

//All am_CPMASkins-enabled code should call this, or the counterpart below to assign model colors
void CG_SetColors(int clientNum, amColorpart_t part, float targetRGB[3])
{
	float colcycle;
	int spectator;

	//No need to bother if CPMA skins are disabled
	if (!am_CPMASkins.integer && part != AM_COLORPART_1 && part != AM_COLORPART_2) return;

	//This function should not be called with clientNum = -1
	if ((clientNum == -1) || (part == AM_COLORPART_NONE)) {
		CG_Printf("Warning: CG_SetColors() called with invalid client/part.\n");
		return;
	}

	//Easter egg!
	if (!strcmp(cgs.clientinfo[clientNum].name, "aib42")) {
		colcycle = (cg.time % 24000) / 4000.0f;

		//aibsmod version info
		if (colcycle < 1.0f) {
			targetRGB[0] = 1.0f;
			targetRGB[1] = colcycle;
			targetRGB[2] = 0.0f;
		} else if (colcycle < 2.0f) {
			targetRGB[0] = 2.0f - colcycle;
			targetRGB[1] = 1.0f;
			targetRGB[2] = 0.0f;
		} else if (colcycle < 3.0f) {
			targetRGB[0] = 0.0f;
			targetRGB[1] = 1.0f;
			targetRGB[2] = colcycle - 2.0f;
		} else if (colcycle < 4.0f) {
			targetRGB[0] = 0.0f;
			targetRGB[1] = 4.0f - colcycle;
			targetRGB[2] = 1.0f;
		} else if (colcycle < 5.0f) {
			targetRGB[0] = colcycle - 4.0f;
			targetRGB[1] = 0.0f;
			targetRGB[2] = 1.0f;
		} else if (colcycle < 6.0f) {
			targetRGB[0] = 1.0f;
			targetRGB[1] = 0.0f;
			targetRGB[2] = 6.0f - colcycle;
		}

		return;
	}

	spectator = (cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR);

	//Find a color based on gametype, overrides, etc.
	if (cgs.gametype < GT_TEAM) { //non-team games
		if (clientNum == cg.clientNum) { //self
			if (!CG_SetColorPart(am_colors.string, part, targetRGB)) {
				if (!CG_SetColorPart(cgs.clientinfo[clientNum].colors, part, targetRGB)) {
					if (part == AM_COLORPART_1) {
						targetRGB[0] = cgs.clientinfo[clientNum].color1[0];
						targetRGB[1] = cgs.clientinfo[clientNum].color1[1];
						targetRGB[2] = cgs.clientinfo[clientNum].color1[2];
					} else if (part == AM_COLORPART_2) {
						targetRGB[0] = cgs.clientinfo[clientNum].color2[0];
						targetRGB[1] = cgs.clientinfo[clientNum].color2[1];
						targetRGB[2] = cgs.clientinfo[clientNum].color2[2];
					} else {
						targetRGB[0] = 1.0f;
						targetRGB[1] = 1.0f;
						targetRGB[2] = 1.0f;
					}
				}
			}
		} else { //enemy
			if (spectator || !CG_SetColorPart(am_enemyColors.string, part, targetRGB)) { //spectator or no am_enemyColors
				if (!CG_SetColorPart(cgs.clientinfo[clientNum].colors, part, targetRGB)) {
					if (part == AM_COLORPART_1) {
						targetRGB[0] = cgs.clientinfo[clientNum].color1[0];
						targetRGB[1] = cgs.clientinfo[clientNum].color1[1];
						targetRGB[2] = cgs.clientinfo[clientNum].color1[2];
					} else if (part == AM_COLORPART_2) {
						targetRGB[0] = cgs.clientinfo[clientNum].color2[0];
						targetRGB[1] = cgs.clientinfo[clientNum].color2[1];
						targetRGB[2] = cgs.clientinfo[clientNum].color2[2];
					} else {
						targetRGB[0] = 1.0f;
						targetRGB[1] = 1.0f;
						targetRGB[2] = 1.0f;
					}
				}
			}
		}
	} else { //team games
		team_t myTeam = cgs.clientinfo[cg.clientNum].team;
		team_t otherTeam = cgs.clientinfo[clientNum].team;

		//friendly color override + am_colors override for color1/color2
		if (!spectator && otherTeam == myTeam) {
			if (!CG_SetColorPart(am_friendlyColors.string, part, targetRGB)) {				//try friendlyColors, if it fails...
				if ((part == AM_COLORPART_1) || (part == AM_COLORPART_2)) 					//and is an effect color,
					if (CG_SetColorPart(cgs.clientinfo[clientNum].colors, part, targetRGB))	//try am_colors
						return;
			} else { //am_friendlyColors succeeded
				return;
			}
		}

		//enemy color override + am_colors override for color1/color2
		else if (!spectator && otherTeam != myTeam) {
			if (!CG_SetColorPart(am_enemyColors.string, part, targetRGB)) {
				if ((part == AM_COLORPART_1) || (part == AM_COLORPART_2))
					if (CG_SetColorPart(cgs.clientinfo[clientNum].colors, part, targetRGB))
						return;
			} else { //am_enemyColors succeeded
				return;
			}
		}

		//If the override didn't work, try these default values:
		if (otherTeam == TEAM_RED) { //default red team color
			if (part == AM_COLORPART_1) {
				targetRGB[0] = cgs.clientinfo[clientNum].color1[0];
				targetRGB[1] = cgs.clientinfo[clientNum].color1[1];
				targetRGB[2] = cgs.clientinfo[clientNum].color1[2];
			} else if (part == AM_COLORPART_2) {
				targetRGB[0] = cgs.clientinfo[clientNum].color2[0];
				targetRGB[1] = cgs.clientinfo[clientNum].color2[1];
				targetRGB[2] = cgs.clientinfo[clientNum].color2[2];
			} else {
				targetRGB[0] = 1.0f;
				targetRGB[1] = 0.0;
				targetRGB[2] = 0.0f;
			}
		} else if (otherTeam == TEAM_BLUE) { //default blue team color
			if (part == AM_COLORPART_1) {
				targetRGB[0] = cgs.clientinfo[clientNum].color1[0];
				targetRGB[1] = cgs.clientinfo[clientNum].color1[1];
				targetRGB[2] = cgs.clientinfo[clientNum].color1[2];
			} else if (part == AM_COLORPART_2) {
				targetRGB[0] = cgs.clientinfo[clientNum].color2[0];
				targetRGB[1] = cgs.clientinfo[clientNum].color2[1];
				targetRGB[2] = cgs.clientinfo[clientNum].color2[2];
			} else {
				targetRGB[0] = 0.0f;
				targetRGB[1] = 0.0;
				targetRGB[2] = 1.0f;
			}
		} else { //this shouldn't happen; use something that will attract attention
			targetRGB[0] = 0.0f;
			targetRGB[1] = 0.0f;
			targetRGB[2] = 0.0f;
		}
	}
}

//Wrapper for above
void CG_SetShaderColors(int clientNum, amColorpart_t part, byte targetRGBA[4])
{
	float rgb[3];

	CG_SetColors(clientNum, part, rgb);

	targetRGBA[0] = (byte) (255.0f * rgb[0]);
	targetRGBA[1] = (byte) (255.0f * rgb[1]);
	targetRGBA[2] = (byte) (255.0f * rgb[2]);

	//Sanitize shader values
	targetRGBA[3] = 255;
	if (targetRGBA[0] < 32) targetRGBA[0] = 24;
	if (targetRGBA[1] < 32) targetRGBA[1] = 24;
	if (targetRGBA[2] < 32) targetRGBA[2] = 24;
}

//Draw a different model, e.g. a redeemer missile
void CG_DifferentModel(centity_t *cent, int renderfx)
{
	refEntity_t	body;
	vec3_t angles;

	memset(&body, 0, sizeof(body));

	//set frame
	body.frame = cent->currentState.frame;
	body.oldframe = body.frame;
	body.backlerp = 0;
	body.renderfx = renderfx;

	VectorCopy(cent->lerpOrigin, body.origin);
	VectorCopy(cent->lerpOrigin, body.oldorigin);
	AnglesToAxis(cent->lerpAngles, body.axis);

	switch (cent->currentState.generic1) {
		case 1: //redeemer missile
			body.hModel = trap_R_RegisterModel("models/ammo/rocket/rocket.md3");

			//look at where we're heading, not where we're looking
			vectoangles(cent->currentState.pos.trDelta, angles);
			AnglesToAxis(angles, body.axis);
			break;
	}

	trap_R_AddRefEntityToScene(&body);
}
