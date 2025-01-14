/*****************************************************************************
 * Name: Molly Johnson, Gaetan Ingrassia, Daniel Rivera
 * File Name: library.cpp
 * Team: Team Runtime Terror
 * Organization: Oregon State University
 * Project Name: Windows Hack Game
 * Created On: 2/4/2020
 * Class: CS467-400 Online Capstone Design
 * William Pfeil - Instructor
 * Prachi Rahurkar - Teachers Assistant (T.A.)
 * Iman Aminzahed - Teachers Assistant (T.A.)
 *****************************************************************************/

/*****************************************************************************
* Description:
* Nop, patch, and DMAAddy functions adapted from:
* https://guidedhacking.com/threads/how-to-hack-any-game-first-internal-hack-dll-tutorial.12142/
****************************************************************************/

// dllmain.cpp : Defines the entry point for the DLL application.
#include <iostream>
#include "mem.h"
#include "library.h"

/****************************************************************************
 * Description: Accepts 2 pointers and check value for activation and deactivation
 * uses memory location to change player health value and NOP function to deactivate 
 * player damage from being taken. function has no return value. moduleBase memory location
 * of start of GameLogic.dll memory block. localPlayerPtr includes offset to module base pointer
 ****************************************************************************/
void cheat::modPlayerHealth(uintptr_t localPlayerPtr, uintptr_t moduleBase, int check_val) {
	if (check_val == 1) {
		std::cout << "Player health changed to 999,999,999" << std::endl;
		uintptr_t healthPtr = mem::FindDMAAddy(localPlayerPtr, { 0x0, 0x0C, 0xE0, 0x294, 0x234, 0x50C, 0xFFFFFFC0 });
		*(int*)(healthPtr) = 999999999;

		//make player invincible by preventing any damage to health by nopping out the player::Damage() function (except for the return)
        mem::Nop((BYTE*)(moduleBase + 0x51150), 6);
        mem::Nop((BYTE*)(moduleBase + 0x5115C), 155);
        mem::Nop((BYTE*)(moduleBase + 0x511FD), 29);
	}
	else {
		std::cout << "Player health changed to 100" << std::endl;
		uintptr_t healthPtr = mem::FindDMAAddy(localPlayerPtr, { 0x0, 0x0C, 0xE0, 0x294, 0x234, 0x50C, 0xFFFFFFC0 });
		*(int*)(healthPtr) = 100;

		//allow player to be damaged again by patching back in the player::Damage() function original code
        mem::Patch((BYTE*)(moduleBase + 0x51150), (BYTE*)"\x55\x8B\xEC\x57\x8B\xF9", 6);
        mem::Patch((BYTE*)(moduleBase + 0x5115C), (BYTE*)"\x85\xC9\x74\x0F\x8B\x01\x8B\x40\x0C\xFF\xD0\x84\xC0\x0F\x84\xA9\x00\x00\x00\x80\xBF\xF0\x00\x00\x00\x00\x0F\x85\x9C\x00\x00\x00\x53\x8B\x5D\x08\x3B\xDF\x75\x16\x66\x0F\x6E\x45\x10\x0F\x5B\xC0\xF3\x0F\x59\x05\xF8\x8A\x2F\x7C\xF3\x0F\x2C\xC0\xEB\x2A\x85\xDB\x74\x23\x8B\x03\x8B\xCB\x8B\x40\x0C\xFF\xD0\x84\xC0\x74\x16\x80\xBF\xE5\x00\x00\x00\x00\x74\x63\x8B\x03\x8B\xCB\x8B\x40\x58\xFF\xD0\x84\xC0\x74\x56\x8B\x45\x10\x56\xFF\x75\x14\x8B\x77\x30\x8B\xCF\x50\xFF\x75\x0C\x89\x45\x10\x53\xE8\x06\x0E\xFB\xFF\x39\x77\x30\x5E\x7D\x0A\xC7\x87\x34\x01\x00\x00\x00\x00\xA0\x41\x8B\x55\x0C\x85\xD2\x74\x26\x83\x7D\x10\x00\x7E\x20", 155);
        mem::Patch((BYTE*)(moduleBase + 0x511FD), (BYTE*)"\x89\x97\xD0\x01\x00\x00\xC7\x87\xD4\x01\x00\x00\x00\x00\xA0\x40\x52\x8B\x01\x57\xFF\x90\x04\x01\x00\x00\x5B\x5F\x5D", 29);
	}
}

/****************************************************************************
 * Description: accepts one uintptr_t parameter for the player pointer and one int parameter
 * for the check value flag. If check val is 1, will use FindDMAAddy to get coin pointer address,
 * then set the coin value pointed to by that pointer to 999,999,999, else if the check val is 0, will use
 * that same method to set the coin value pointed to by that pointer to 100 (default). This function
 * not used since the ReceiveAllItems hack provides the user with the max value of Pwncoins.
 * NON - FUNCTIONAL
 ****************************************************************************/
/*
void cheat::AddCoins(uintptr_t localPlayerPtr, int check_val) {
	if (check_val == 1) {
		std::cout << "Player coin amount changed to 999,999,999" << std::endl;
		uintptr_t coinPtr = mem::FindDMAAddy(localPlayerPtr, { 0x4, 0x0, 0x10, 0x4C, 0x4, 0x0, 0x18 });
		*(int*)(coinPtr) = 999999999;
	}
	else {
		std::cout << "Player coin amount changed to 100" << std::endl;
		uintptr_t coinPtr = mem::FindDMAAddy(localPlayerPtr, { 0x4, 0x0, 0x10, 0x4C, 0x4, 0x0, 0x18 });
		*(int*)(coinPtr) = 100;
	}
}
*/

/****************************************************************************
 * Description: Accepts two uintptr_t parameters, one for the base memory location
 * of the player attributes and one for the base of GameLogic.dll. It also takes
 * the integer value for the check value flag. If check vak is 1, it sets the player mana
 * to a value of 999,999,999 and NOPs the code for the function Player::UseMana, making
 * it so no mana is lost when using a spell. If the check val is zero, mana behavior
 * and value is reset to the default.
 ****************************************************************************/
void cheat::modPlayerMana(uintptr_t localPlayerPtr, uintptr_t moduleBase, int check_val) {
	if (check_val == 1) {
		std::cout << "Player mana cheat activated" << std::endl;
		uintptr_t manaPtr = mem::FindDMAAddy(localPlayerPtr, { 0x0, 0x0C, 0xE0, 0x294, 0x234, 0x50C, 0xBC });
		*(int*)(manaPtr) = 999999999;

		DWORD nopAddr = moduleBase + 0x525C1;
		mem::Nop((BYTE*)nopAddr, 6);
	}
	else {
		std::cout << "Player mana cheat deactivated" << std::endl;
		uintptr_t manaPtr = mem::FindDMAAddy(localPlayerPtr, { 0x0, 0x0C, 0xE0, 0x294, 0x234, 0x50C, 0xBC });
		*(int*)(manaPtr) = 100;

		// reset useMana - this changes is back to the orginal code
		DWORD useManaAddr = moduleBase + 0x525C1;
		mem::Patch((BYTE*)useManaAddr, (BYTE*)"\x3b\xc2\x7c\xe6\x2b\xc2", 6);
	}
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the game module base (GameLogic.dll plus
 * the offset of the pwnie island function portion being modified) and a check val flag. If the check
 * val is 1, will nop 2 bytes of code that cause a sold item to be removed from inventory or pwncoins
 * to be removed from inventory when an item is purchased, causing the user to be able to keep all
 * items like weapons and ammo as well as pwncoins when buying and selling. This allows the user
 * to sell weapons/ammo to increase pwncoins without losing the sold weapons/ammo, and to purchase
 * weapons/ammo without losing pwncoins from the purchase. If the check val is 0, will patch back in
 * the original bytes (in hex), causing the function to work normally and pwncoins to be spent when ammo/
 * weapons are purchased and ammo/weapons removed from inventory when sold for pwncoins. Function
 * does not return anything.
 ****************************************************************************/
void cheat::KeepItems(uintptr_t moduleBase, int check_val)
{
	//if hack is toggled on
	if (check_val == 1)
	{
		//keep pwncoins and ammo even when you spend pwncoins or sell ammo
		//by nopping out the code that subtracts from your current inventory item (ammo or pwncoins)
		mem::Nop((BYTE*)(moduleBase + 0x52215), 2);
		std::cout << "Keep items cheat activated" << std::endl;
	}
	//if hack is toggled off
	else
	{
		//patch back in the original code that subtracts from your current inventory item (ammo or pwncoins)
		mem::Patch((BYTE*)(moduleBase + 0x52215), (BYTE*)"\x2B\xCF", 2);
		std::cout << "Keep items cheat deactivated" << std::endl;
	}
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the game module base (GameLogic.dll plus
 * the offset of the pwnie island function portion being modified) and a check val flag.
 * Function does not return anything. If the check val is 1, will patch in code for each weapon's
 * getDamage function in pwnie island such that the physical damage is set to 20000 instead of
 * whatever that weapon's default physical damage value was. If the check val is 0, will patch
 * back in the original code in hex for each weapon's getDamage function so the physical damage
 * for each weapon is set back to its default value.
 ****************************************************************************/
void cheat::IncreasedGunDamage(uintptr_t moduleBase, int check_val)
{
	//if hack is toggled on
	if (check_val == 1)
	{
		//set pistol damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x13930), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set AKrifle damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x139F0), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set cowboy coder damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x13AA0), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set remote exploit (unbearable revenge quest reward rifle) damage  to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x13C00), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set legendary holy hand grenade damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x13F60), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set legendary golden master revolver damage to 20000 via patch 
		mem::Patch((BYTE*)(moduleBase + 0x51B0), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set hand cannon damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x141B0), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set chain gun damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x13C90), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set heap spray damage to 20000 via patch (this one uses a x12 multiplier so won't equal 20000 exactly but be very close)
		mem::Patch((BYTE*)(moduleBase + 0x51A0), (BYTE*)"\xB8\x83\x06\x00\x00", 5);

		std::cout << "Increase gun damage cheat activated" << std::endl;
	}
	//if hack is toggled off
	else
	{
		//set pistol damage back to default via patch of original code
		mem::Patch((BYTE*)(moduleBase + 0x13930), (BYTE*)"\xB8\x0E\x00\x00\x00", 5);

		//set AKrifle damage back to default via patch of original code
		mem::Patch((BYTE*)(moduleBase + 0x139F0), (BYTE*)"\xB8\x0B\x00\x00\x00", 5);

		//set cowboy coder damage back to default via patch of original code
		mem::Patch((BYTE*)(moduleBase + 0x13AA0), (BYTE*)"\xB8\x3C\x00\x00\x00", 5);

		//set remote exploit (unbearable revenge quest reward rifle)damage back to default via patch
		mem::Patch((BYTE*)(moduleBase + 0x13C00), (BYTE*)"\xB8\x96\x00\x00\x00", 5 );

		//set legendary holy hand grenade damage back to default via patch
		mem::Patch((BYTE*)(moduleBase + 0x13F60), (BYTE*)"\xB8\x2C\x01\x00\x00", 5);

		//set legendary golden master revolver damage back to default via patch
		mem::Patch((BYTE*)(moduleBase + 0x51B0), (BYTE*)"\xB8\x7D\x00\x00\x00", 5);

        //set hand cannon damage back to default via patch
		mem::Patch((BYTE*)(moduleBase + 0x141B0), (BYTE*)"\xB8\x78\x00\x00\x00", 5);

		//set chain gun damage back to default via patch
		mem::Patch((BYTE*)(moduleBase + 0x13C90), (BYTE*)"\xB8\x12\x00\x00\x00", 5);

		//set heap spray damage back to default  via patch
		mem::Patch((BYTE*)(moduleBase + 0x51A0), (BYTE*)"\xB8\x14\x00\x00\x00", 5);

		std::cout << "Increase gun damage cheat deactivated" << std::endl;
	}
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the game module base (GameLogic.dll plus
 * the offset of the pwnie island function portion being modified) and a check val flag.
 * Function does not return anything. If the check val is 1, will patch in code for each spell's
 * getDamage function in pwnie island such that the physical damage is set to 20000 instead of
 * whatever that spell's default physical damage value was. If the check val is 0, will patch
 * back in the original code in hex for each spell's getDamage function so the physical damage
 * for each spell is set back to its default value. 
 ****************************************************************************/
void cheat::IncreasedSpellDamage(uintptr_t moduleBase, int check_val)
{
	//if hack is toggled on
	if (check_val == 1)
	{
		//set balls of fire damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0xD7E0), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set zero cool damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x136F0), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set legendary char star damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x135B0), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		//set static link damage to 20000 via patch
		mem::Patch((BYTE*)(moduleBase + 0x14020), (BYTE*)"\xB8\x20\x4E\x00\x00", 5);

		std::cout << "Increase spell damage cheat activated" << std::endl;
	}
	//if hack is toggled off
	else
	{
		//set balls of fire damage back to default via patch of the original code
		mem::Patch((BYTE*)(moduleBase + 0xD7E0), (BYTE*)"\xB8\x19\x00\x00\x00", 5);

		//set zero cool damage back to default via patch of the original code
		mem::Patch((BYTE*)(moduleBase + 0x136F0), (BYTE*)"\xB8\x20\x00\x00\x00", 5);

		//set legendary char star damage back to default via patch of the original code
		mem::Patch((BYTE*)(moduleBase + 0x135B0), (BYTE*)"\xB8\x50\x00\x00\x00", 5);

		//set static link damage back to default via patch of the original code
		mem::Patch((BYTE*)(moduleBase + 0x14020), (BYTE*)"\xB8\x0F\x00\x00\x00", 5);


		std::cout << "Increase spell damage cheat deactivated" << std::endl;
	}
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the game module base (GameLogic.dll plus
 * the offset of the pwnie island function portion being modified) and a check val flag.
 * Function does not return anything. If the check val is 1, will nop 2 bytes of code that cause
 * any weapon's currently loaded clip of ammo to decrement for each shot fired, thus allowing
 * the user to shoot without any weapon losing ammo from their current loaded clip. Allows
 * the user to never have to reload. If the check val is 0, will patch back in the original code
 * that causes the ammo from the current loaded clip to be decremented with each shot (i.e.
 * the default behavior).
 ****************************************************************************/
void cheat::UnlimitedAmmo(uintptr_t moduleBase, int check_val)
{
	//if hack is toggled on
	if (check_val == 1)
	{
		//give unlimited ammo for all guns by by nopping out the code that subtracts from your current loaded ammo
		mem::Nop((BYTE*)(moduleBase + 0x52394), 2);

		std::cout << "Unlimited weapon ammo cheat activated" << std::endl;
	}
	//if hack is toggled off
	else
	{
		//put back in the code that subtracts from your current loaded ammo via patch of the original code 
		mem::Patch((BYTE*)(moduleBase + 0x52394), (BYTE*)"\x2B\xCF", 2);

		std::cout << "Unlimited weapon ammo cheat deactivated" << std::endl;
	}
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the base memory location
 * of the player attributes and the integer values for the speed modifier.
 * It then sets the player walk speed to this value using the pointer to the
 * value in memory.
 ****************************************************************************/
void cheat::modWalkSpeed(uintptr_t localPlayerPtr1, int speed_modifier) {
		uintptr_t walkSpeed = mem::FindDMAAddy(localPlayerPtr1, { 0x4 , 0x8, 0x4, 0x4, 0x10, 0x120 }); //0x97E48
		float* wsPtr = (float*)walkSpeed;
		*wsPtr = (float)200.0 * speed_modifier;
}


/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the base memory location
 * of the player attributes and the integer values for the speed modifier.
 * It then sets the player jump speed to this value using the pointer to the
 * value in memory.
 ****************************************************************************/
void cheat::increaseJumpSpeed(uintptr_t localPlayerPtr1, int speed_modifier) {
		uintptr_t jumpSpeed = mem::FindDMAAddy(localPlayerPtr1, { 0x4 , 0x8, 0x10, 0x124 }); //0x97E48
		float* jsPtr = (float*)jumpSpeed;
		*jsPtr = (float)420.0 * speed_modifier;
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the base memory location
 * of the player attributes and the integer value for the check val.
 * It then sets the player jump hold time to 10,000 if the check val is 1
 * or the default value if the check val is 0. It does this using the pointer to the
 * value in memory.
 ****************************************************************************/
void cheat::increaseJumpHoldTime(uintptr_t localPlayerPtr1, int check_val) {
		uintptr_t jumpHoldTime = mem::FindDMAAddy(localPlayerPtr1, { 0x1C , 0x4, 0x224, 0x30, 0x18, 0x3E0, 0x128 }); //0x97E48
		float* jhtPtr = (float*)jumpHoldTime;
		if (check_val == 1) {
			*jhtPtr = (float)10000.0;
		}
		else {
			*jhtPtr = (float)0.200000003;
		}
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the base memory location
 * of the player coordinates and the float values for the x, y, and z coordinates.
 * It then sets the player coordinates to these values using pointers to their
 * values in memory.
 ****************************************************************************/
void cheat::Teleport(uintptr_t localPlayerPtr, float xPosVal, float yPosVal, float zPosVal) {
	/*
	Starting Position: x:-53090, y:-57298, z:1027
	Fireball: x:-43651, y:-55970, z:324
	Starting Buildings: x:-39456, y:-21931, z:2796
	*/

	std::cout << "Teleporting to (" << xPosVal << ", " << yPosVal << ", " << zPosVal << ")" << std::endl;
	uintptr_t xPosAddr = mem::FindDMAAddy(localPlayerPtr, { 0x1C , 0x4, 0x254, 0x238, 0x27C, 0x74, 0x90 }); //0x97E1C 0x24 , 0xC, 0xC8, 0x424, 0x184, 0x74, 0x90
	float* xPos = (float*)xPosAddr;
	*xPos = xPosVal;

	uintptr_t yPosAddr = mem::FindDMAAddy(localPlayerPtr, { 0x1C , 0x4, 0x254, 0x238, 0x27C, 0x74, 0x94 }); //0x97E1C 0x24 , 0xC, 0xC8, 0x424, 0x184, 0x74, 0x94
	float* yPos = (float*)yPosAddr;
	*yPos = yPosVal;

	uintptr_t zPosAddr = mem::FindDMAAddy(localPlayerPtr, { 0x1C , 0x4, 0x254, 0x238, 0x27C, 0x74, 0x98 }); //0x97E1C 0x24 , 0xC, 0xC8, 0x424, 0x184, 0x74, 0x98
	float* zPos = (float*)zPosAddr;
	*zPos = zPosVal;
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the game module base (GameLogic.dll plus
 * the offset of the pwnie island function portion being modified) and a check val flag.
 * Function does not return anything. If the check val is 1, will patch in code causing the game's
 * IsItemOnCooldown function to always return false (i.e. so no item is ever on cooldown). If
 * the check val is 0, will patch back in the original code causing the default behavior to occur
 * again (i.e. items can be on cooldown again, so there can be forced cooldown time intervals
 * between shots fired for weapons or between the casting of spells).
 ****************************************************************************/
void cheat::NoItemCooldown(uintptr_t moduleBase, int check_val) {
	//if hack is toggled on
	if (check_val == 1)
	{
		//set IsItemOnCooldown to false via a patch
		mem::Patch((BYTE*)(moduleBase + 0x526B2), (BYTE*)"\xB0\x00", 2);
	}
	//if hack is toggled off
	else
	{
		//set IsItemOnCooldown back to default (true) via a patch
		mem::Patch((BYTE*)(moduleBase + 0x526B2), (BYTE*)"\xB0\x01", 2);
	}
}

/****************************************************************************
 * Description: Accepts one uintptr_t parameter for the game module base (GameLogic.dll) 
 * and an int check value flag. The function does not return anything. If the check val  
 * is 1, it will patch the Player::GetWalkingSpeed function to call the GameAPI::GiveAll 
 * function. This will give the player the max number of each attainable item. If the check 
 * val is 0, it will patch back in the original code causing the default function behavior.
 ****************************************************************************/
void cheat::GiveAllItems(uintptr_t moduleBase, int check_val) {
	//if hack is toggled on
	if (check_val == 1)
	{
		// Give All Guns
		mem::Patch((BYTE*)(moduleBase + 0x4FF90), (BYTE*)"\xD9\x81\x20\x01\x00\x00\x51\xE8\xA4\xE6\xFC\xFF\xC3", 13);		
	}
	//if hack is toggled off
	else
	{
		// Reset Walking Speed
		mem::Patch((BYTE*)(moduleBase + 0x4FF90), (BYTE*)"\xD9\x81\x20\x01\x00\x00\xC3", 7);
	}
}
