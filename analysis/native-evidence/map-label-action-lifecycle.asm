; manager vector helper: count is (end-begin)/4

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  9bb920: 55                           	push	ebp
  9bb921: 8b ec                        	mov	ebp, esp
  9bb923: 51                           	push	ecx
  9bb924: 89 4d fc                     	mov	dword ptr [ebp - 0x4], ecx
  9bb927: 8b 45 fc                     	mov	eax, dword ptr [ebp - 0x4]
  9bb92a: 8b 4d fc                     	mov	ecx, dword ptr [ebp - 0x4]
  9bb92d: 8b 40 04                     	mov	eax, dword ptr [eax + 0x4]
  9bb930: 2b 01                        	sub	eax, dword ptr [ecx]
  9bb932: c1 f8 02                     	sar	eax, 0x2
  9bb935: 8b e5                        	mov	esp, ebp
  9bb937: 5d                           	pop	ebp
  9bb938: c3                           	ret

; vector index helper returns begin + 4*index

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  9b8a30: 55                           	push	ebp
  9b8a31: 8b ec                        	mov	ebp, esp
  9b8a33: 51                           	push	ecx
  9b8a34: 89 4d fc                     	mov	dword ptr [ebp - 0x4], ecx
  9b8a37: 8b 45 fc                     	mov	eax, dword ptr [ebp - 0x4]
  9b8a3a: 8b 08                        	mov	ecx, dword ptr [eax]
  9b8a3c: 8b 55 08                     	mov	edx, dword ptr [ebp + 0x8]
  9b8a3f: 8d 04 91                     	lea	eax, [ecx + 4*edx]
  9b8a42: 8b e5                        	mov	esp, ebp
  9b8a44: 5d                           	pop	ebp
  9b8a45: c2 04 00                     	ret	0x4

; manager action loop: calls 0x50f980 and erases entries that return true

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  5110e0: 55                           	push	ebp
  5110e1: 8b ec                        	mov	ebp, esp
  5110e3: 83 ec 08                     	sub	esp, 0x8
  5110e6: 89 4d f8                     	mov	dword ptr [ebp - 0x8], ecx
  5110e9: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  5110ec: 83 c1 14                     	add	ecx, 0x14
  5110ef: e8 ec 82 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  5110f4: 0f b6 c0                     	movzx	eax, al
  5110f7: 85 c0                        	test	eax, eax
  5110f9: 0f 85 94 00 00 00            	jne	0x511193 <PHYSFSRWOPS_openAppend+0x10d703>
  5110ff: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  511102: 83 c1 34                     	add	ecx, 0x34
  511105: e8 06 09 f1 ff               	call	0x421a10 <PHYSFSRWOPS_openAppend+0x1df80>
  51110a: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  51110d: 83 c1 14                     	add	ecx, 0x14
  511110: e8 0b a8 4a 00               	call	0x9bb920 <PHYSFSRWOPS_openAppend+0x5b7e90>
  511115: 83 e8 01                     	sub	eax, 0x1
  511118: 89 45 fc                     	mov	dword ptr [ebp - 0x4], eax
  51111b: eb 09                        	jmp	0x511126 <PHYSFSRWOPS_openAppend+0x10d696>
  51111d: 8b 4d fc                     	mov	ecx, dword ptr [ebp - 0x4]
  511120: 83 e9 01                     	sub	ecx, 0x1
  511123: 89 4d fc                     	mov	dword ptr [ebp - 0x4], ecx
  511126: 83 7d fc 00                  	cmp	dword ptr [ebp - 0x4], 0x0
  51112a: 7c 61                        	jl	0x51118d <PHYSFSRWOPS_openAppend+0x10d6fd>
  51112c: 8b 55 fc                     	mov	edx, dword ptr [ebp - 0x4]
  51112f: 52                           	push	edx
  511130: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  511133: 83 c1 14                     	add	ecx, 0x14
  511136: e8 f5 78 4a 00               	call	0x9b8a30 <PHYSFSRWOPS_openAppend+0x5b4fa0>
  51113b: 8b 08                        	mov	ecx, dword ptr [eax]
  51113d: e8 3e e8 ff ff               	call	0x50f980 <PHYSFSRWOPS_openAppend+0x10bef0>
  511142: 0f b6 c0                     	movzx	eax, al
  511145: 85 c0                        	test	eax, eax
  511147: 74 2e                        	je	0x511177 <PHYSFSRWOPS_openAppend+0x10d6e7>
  511149: 8b 4d fc                     	mov	ecx, dword ptr [ebp - 0x4]
  51114c: 51                           	push	ecx
  51114d: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  511150: 83 c1 14                     	add	ecx, 0x14
  511153: e8 d8 78 4a 00               	call	0x9b8a30 <PHYSFSRWOPS_openAppend+0x5b4fa0>
  511158: 50                           	push	eax
  511159: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  51115c: 83 c1 24                     	add	ecx, 0x24
  51115f: e8 1c 97 4d 00               	call	0x9ea880 <PHYSFSRWOPS_openAppend+0x5e6df0>
  511164: 8b 55 fc                     	mov	edx, dword ptr [ebp - 0x4]
  511167: 52                           	push	edx
  511168: 8b 45 f8                     	mov	eax, dword ptr [ebp - 0x8]
  51116b: 83 c0 14                     	add	eax, 0x14
  51116e: 50                           	push	eax
  51116f: e8 dc 2f 4d 00               	call	0x9e4150 <PHYSFSRWOPS_openAppend+0x5e06c0>
  511174: 83 c4 08                     	add	esp, 0x8
  511177: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  51117a: 83 c1 14                     	add	ecx, 0x14
  51117d: e8 5e 82 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  511182: 0f b6 c8                     	movzx	ecx, al
  511185: 85 c9                        	test	ecx, ecx
  511187: 74 02                        	je	0x51118b <PHYSFSRWOPS_openAppend+0x10d6fb>
  511189: eb 02                        	jmp	0x51118d <PHYSFSRWOPS_openAppend+0x10d6fd>
  51118b: eb 90                        	jmp	0x51111d <PHYSFSRWOPS_openAppend+0x10d68d>
  51118d: b0 01                        	mov	al, 0x1
  51118f: eb 04                        	jmp	0x511195 <PHYSFSRWOPS_openAppend+0x10d705>
  511191: eb 02                        	jmp	0x511195 <PHYSFSRWOPS_openAppend+0x10d705>
  511193: 32 c0                        	xor	al, al
  511195: 8b e5                        	mov	esp, ebp
  511197: 5d                           	pop	ebp
  511198: c3                           	ret
  511199: cc                           	int3

; action update clear paths; include final return 0x5105d2

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  50f980: 55                           	push	ebp
  50f981: 8b ec                        	mov	ebp, esp
  50f983: 83 ec 78                     	sub	esp, 0x78
  50f986: 56                           	push	esi
  50f987: 89 4d e8                     	mov	dword ptr [ebp - 0x18], ecx
  50f98a: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50f98d: 83 78 08 00                  	cmp	dword ptr [eax + 0x8], 0x0
  50f991: 75 07                        	jne	0x50f99a <PHYSFSRWOPS_openAppend+0x10bf0a>
  50f993: 32 c0                        	xor	al, al
  50f995: e9 38 0c 00 00               	jmp	0x5105d2 <PHYSFSRWOPS_openAppend+0x10cb42>
  50f99a: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50f99d: 83 79 0c 00                  	cmp	dword ptr [ecx + 0xc], 0x0
  50f9a1: 0f 84 08 01 00 00            	je	0x50faaf <PHYSFSRWOPS_openAppend+0x10c01f>
  50f9a7: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50f9aa: a1 f0 fd ca 00               	mov	eax, dword ptr [0xcafdf0]
  50f9af: 3b 42 0c                     	cmp	eax, dword ptr [edx + 0xc]
  50f9b2: 0f 82 f7 00 00 00            	jb	0x50faaf <PHYSFSRWOPS_openAppend+0x10c01f>
  50f9b8: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50f9bb: 8b 11                        	mov	edx, dword ptr [ecx]
  50f9bd: 83 7a 28 03                  	cmp	dword ptr [edx + 0x28], 0x3
  50f9c1: 75 1d                        	jne	0x50f9e0 <PHYSFSRWOPS_openAppend+0x10bf50>
  50f9c3: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50f9c6: 83 c0 68                     	add	eax, 0x68
  50f9c9: 50                           	push	eax
  50f9ca: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50f9cd: 83 c1 10                     	add	ecx, 0x10
  50f9d0: e8 0b a4 ef ff               	call	0x409de0 <PHYSFSRWOPS_openAppend+0x6350>
  50f9d5: 0f b6 c8                     	movzx	ecx, al
  50f9d8: 85 c9                        	test	ecx, ecx
  50f9da: 0f 85 cf 00 00 00            	jne	0x50faaf <PHYSFSRWOPS_openAppend+0x10c01f>
  50f9e0: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50f9e3: 8b 0a                        	mov	ecx, dword ptr [edx]
  50f9e5: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  50f9eb: e8 f0 99 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50f9f0: 0f b6 c0                     	movzx	eax, al
  50f9f3: 85 c0                        	test	eax, eax
  50f9f5: 75 11                        	jne	0x50fa08 <PHYSFSRWOPS_openAppend+0x10bf78>
  50f9f7: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50f9fa: 83 c1 10                     	add	ecx, 0x10
  50f9fd: 51                           	push	ecx
  50f9fe: 6a 05                        	push	0x5
  50fa00: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fa03: e8 b8 e5 ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  50fa08: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fa0b: 8b 0a                        	mov	ecx, dword ptr [edx]
  50fa0d: 81 c1 dc 00 00 00            	add	ecx, 0xdc
  50fa13: e8 c8 99 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fa18: 0f b6 c0                     	movzx	eax, al
  50fa1b: 85 c0                        	test	eax, eax
  50fa1d: 75 15                        	jne	0x50fa34 <PHYSFSRWOPS_openAppend+0x10bfa4>
  50fa1f: 0f b6 0d 94 9d d2 00         	movzx	ecx, byte ptr [0xd29d94]
  50fa26: 85 c9                        	test	ecx, ecx
  50fa28: 75 0a                        	jne	0x50fa34 <PHYSFSRWOPS_openAppend+0x10bfa4>
  50fa2a: 6a 02                        	push	0x2
  50fa2c: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fa2f: e8 7c ed ff ff               	call	0x50e7b0 <PHYSFSRWOPS_openAppend+0x10ad20>
  50fa34: 33 d2                        	xor	edx, edx
  50fa36: 75 d0                        	jne	0x50fa08 <PHYSFSRWOPS_openAppend+0x10bf78>
  50fa38: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fa3b: 8b 08                        	mov	ecx, dword ptr [eax]
  50fa3d: 81 c1 f0 00 00 00            	add	ecx, 0xf0
  50fa43: e8 98 99 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fa48: 0f b6 c8                     	movzx	ecx, al
  50fa4b: 85 c9                        	test	ecx, ecx
  50fa4d: 75 0a                        	jne	0x50fa59 <PHYSFSRWOPS_openAppend+0x10bfc9>
  50fa4f: 6a 02                        	push	0x2
  50fa51: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fa54: e8 87 ee ff ff               	call	0x50e8e0 <PHYSFSRWOPS_openAppend+0x10ae50>
  50fa59: 33 d2                        	xor	edx, edx
  50fa5b: 75 db                        	jne	0x50fa38 <PHYSFSRWOPS_openAppend+0x10bfa8>
  50fa5d: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fa60: 8b 08                        	mov	ecx, dword ptr [eax]
  50fa62: 83 b9 b4 00 00 00 00         	cmp	dword ptr [ecx + 0xb4], 0x0
  50fa69: 75 1c                        	jne	0x50fa87 <PHYSFSRWOPS_openAppend+0x10bff7>
  50fa6b: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fa6e: 8b 02                        	mov	eax, dword ptr [edx]
  50fa70: 83 b8 bc 00 00 00 00         	cmp	dword ptr [eax + 0xbc], 0x0
  50fa77: 75 0e                        	jne	0x50fa87 <PHYSFSRWOPS_openAppend+0x10bff7>
  50fa79: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fa7c: 8b 11                        	mov	edx, dword ptr [ecx]
  50fa7e: 83 ba c4 00 00 00 00         	cmp	dword ptr [edx + 0xc4], 0x0
  50fa85: 74 0f                        	je	0x50fa96 <PHYSFSRWOPS_openAppend+0x10c006>
  50fa87: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fa8a: 83 c0 10                     	add	eax, 0x10
  50fa8d: 50                           	push	eax
  50fa8e: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fa91: e8 aa f0 ff ff               	call	0x50eb40 <PHYSFSRWOPS_openAppend+0x10b0b0>
  50fa96: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fa99: c7 41 08 00 00 00 00         	mov	dword ptr [ecx + 0x8], 0x0
  50faa0: b0 01                        	mov	al, 0x1
  50faa2: e9 2b 0b 00 00               	jmp	0x5105d2 <PHYSFSRWOPS_openAppend+0x10cb42>
  50faa7: 33 d2                        	xor	edx, edx
  50faa9: 0f 85 31 ff ff ff            	jne	0x50f9e0 <PHYSFSRWOPS_openAppend+0x10bf50>
  50faaf: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fab2: 8b 08                        	mov	ecx, dword ptr [eax]
  50fab4: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  50faba: e8 21 99 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fabf: 0f b6 c8                     	movzx	ecx, al
  50fac2: 85 c9                        	test	ecx, ecx
  50fac4: 0f 85 eb 00 00 00            	jne	0x50fbb5 <PHYSFSRWOPS_openAppend+0x10c125>
  50faca: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50facd: 83 c2 10                     	add	edx, 0x10
  50fad0: 52                           	push	edx
  50fad1: 6a 00                        	push	0x0
  50fad3: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fad6: e8 e5 e4 ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  50fadb: 0f b6 c0                     	movzx	eax, al
  50fade: 85 c0                        	test	eax, eax
  50fae0: 0f 84 cf 00 00 00            	je	0x50fbb5 <PHYSFSRWOPS_openAppend+0x10c125>
  50fae6: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fae9: 8b 09                        	mov	ecx, dword ptr [ecx]
  50faeb: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  50faf1: e8 ea 98 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50faf6: 0f b6 d0                     	movzx	edx, al
  50faf9: 85 d2                        	test	edx, edx
  50fafb: 75 11                        	jne	0x50fb0e <PHYSFSRWOPS_openAppend+0x10c07e>
  50fafd: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fb00: 83 c0 10                     	add	eax, 0x10
  50fb03: 50                           	push	eax
  50fb04: 6a 05                        	push	0x5
  50fb06: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fb09: e8 b2 e4 ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  50fb0e: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fb11: 8b 09                        	mov	ecx, dword ptr [ecx]
  50fb13: 81 c1 dc 00 00 00            	add	ecx, 0xdc
  50fb19: e8 c2 98 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fb1e: 0f b6 d0                     	movzx	edx, al
  50fb21: 85 d2                        	test	edx, edx
  50fb23: 75 15                        	jne	0x50fb3a <PHYSFSRWOPS_openAppend+0x10c0aa>
  50fb25: 0f b6 05 94 9d d2 00         	movzx	eax, byte ptr [0xd29d94]
  50fb2c: 85 c0                        	test	eax, eax
  50fb2e: 75 0a                        	jne	0x50fb3a <PHYSFSRWOPS_openAppend+0x10c0aa>
  50fb30: 6a 02                        	push	0x2
  50fb32: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fb35: e8 76 ec ff ff               	call	0x50e7b0 <PHYSFSRWOPS_openAppend+0x10ad20>
  50fb3a: 33 c9                        	xor	ecx, ecx
  50fb3c: 75 d0                        	jne	0x50fb0e <PHYSFSRWOPS_openAppend+0x10c07e>
  50fb3e: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fb41: 8b 0a                        	mov	ecx, dword ptr [edx]
  50fb43: 81 c1 f0 00 00 00            	add	ecx, 0xf0
  50fb49: e8 92 98 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fb4e: 0f b6 c0                     	movzx	eax, al
  50fb51: 85 c0                        	test	eax, eax
  50fb53: 75 0a                        	jne	0x50fb5f <PHYSFSRWOPS_openAppend+0x10c0cf>
  50fb55: 6a 02                        	push	0x2
  50fb57: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fb5a: e8 81 ed ff ff               	call	0x50e8e0 <PHYSFSRWOPS_openAppend+0x10ae50>
  50fb5f: 33 c9                        	xor	ecx, ecx
  50fb61: 75 db                        	jne	0x50fb3e <PHYSFSRWOPS_openAppend+0x10c0ae>
  50fb63: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fb66: 8b 02                        	mov	eax, dword ptr [edx]
  50fb68: 83 b8 b4 00 00 00 00         	cmp	dword ptr [eax + 0xb4], 0x0
  50fb6f: 75 1c                        	jne	0x50fb8d <PHYSFSRWOPS_openAppend+0x10c0fd>
  50fb71: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fb74: 8b 11                        	mov	edx, dword ptr [ecx]
  50fb76: 83 ba bc 00 00 00 00         	cmp	dword ptr [edx + 0xbc], 0x0
  50fb7d: 75 0e                        	jne	0x50fb8d <PHYSFSRWOPS_openAppend+0x10c0fd>
  50fb7f: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fb82: 8b 08                        	mov	ecx, dword ptr [eax]
  50fb84: 83 b9 c4 00 00 00 00         	cmp	dword ptr [ecx + 0xc4], 0x0
  50fb8b: 74 0f                        	je	0x50fb9c <PHYSFSRWOPS_openAppend+0x10c10c>
  50fb8d: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fb90: 83 c2 10                     	add	edx, 0x10
  50fb93: 52                           	push	edx
  50fb94: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fb97: e8 a4 ef ff ff               	call	0x50eb40 <PHYSFSRWOPS_openAppend+0x10b0b0>
  50fb9c: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fb9f: c7 40 08 00 00 00 00         	mov	dword ptr [eax + 0x8], 0x0
  50fba6: b0 01                        	mov	al, 0x1
  50fba8: e9 25 0a 00 00               	jmp	0x5105d2 <PHYSFSRWOPS_openAppend+0x10cb42>
  50fbad: 33 c9                        	xor	ecx, ecx
  50fbaf: 0f 85 31 ff ff ff            	jne	0x50fae6 <PHYSFSRWOPS_openAppend+0x10c056>
  50fbb5: 33 d2                        	xor	edx, edx
  50fbb7: 0f 85 f2 fe ff ff            	jne	0x50faaf <PHYSFSRWOPS_openAppend+0x10c01f>
  50fbbd: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fbc0: 8b 08                        	mov	ecx, dword ptr [eax]
  50fbc2: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  50fbc8: e8 13 98 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fbcd: 0f b6 c8                     	movzx	ecx, al
  50fbd0: 85 c9                        	test	ecx, ecx
  50fbd2: 0f 85 eb 00 00 00            	jne	0x50fcc3 <PHYSFSRWOPS_openAppend+0x10c233>
  50fbd8: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fbdb: 83 c2 10                     	add	edx, 0x10
  50fbde: 52                           	push	edx
  50fbdf: 6a 01                        	push	0x1
  50fbe1: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fbe4: e8 d7 e3 ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  50fbe9: 0f b6 c0                     	movzx	eax, al
  50fbec: 85 c0                        	test	eax, eax
  50fbee: 0f 84 cf 00 00 00            	je	0x50fcc3 <PHYSFSRWOPS_openAppend+0x10c233>
  50fbf4: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fbf7: 8b 09                        	mov	ecx, dword ptr [ecx]
  50fbf9: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  50fbff: e8 dc 97 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fc04: 0f b6 d0                     	movzx	edx, al
  50fc07: 85 d2                        	test	edx, edx
  50fc09: 75 11                        	jne	0x50fc1c <PHYSFSRWOPS_openAppend+0x10c18c>
  50fc0b: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fc0e: 83 c0 10                     	add	eax, 0x10
  50fc11: 50                           	push	eax
  50fc12: 6a 05                        	push	0x5
  50fc14: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fc17: e8 a4 e3 ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  50fc1c: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fc1f: 8b 09                        	mov	ecx, dword ptr [ecx]
  50fc21: 81 c1 dc 00 00 00            	add	ecx, 0xdc
  50fc27: e8 b4 97 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fc2c: 0f b6 d0                     	movzx	edx, al
  50fc2f: 85 d2                        	test	edx, edx
  50fc31: 75 15                        	jne	0x50fc48 <PHYSFSRWOPS_openAppend+0x10c1b8>
  50fc33: 0f b6 05 94 9d d2 00         	movzx	eax, byte ptr [0xd29d94]
  50fc3a: 85 c0                        	test	eax, eax
  50fc3c: 75 0a                        	jne	0x50fc48 <PHYSFSRWOPS_openAppend+0x10c1b8>
  50fc3e: 6a 02                        	push	0x2
  50fc40: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fc43: e8 68 eb ff ff               	call	0x50e7b0 <PHYSFSRWOPS_openAppend+0x10ad20>
  50fc48: 33 c9                        	xor	ecx, ecx
  50fc4a: 75 d0                        	jne	0x50fc1c <PHYSFSRWOPS_openAppend+0x10c18c>
  50fc4c: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fc4f: 8b 0a                        	mov	ecx, dword ptr [edx]
  50fc51: 81 c1 f0 00 00 00            	add	ecx, 0xf0
  50fc57: e8 84 97 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fc5c: 0f b6 c0                     	movzx	eax, al
  50fc5f: 85 c0                        	test	eax, eax
  50fc61: 75 0a                        	jne	0x50fc6d <PHYSFSRWOPS_openAppend+0x10c1dd>
  50fc63: 6a 02                        	push	0x2
  50fc65: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fc68: e8 73 ec ff ff               	call	0x50e8e0 <PHYSFSRWOPS_openAppend+0x10ae50>
  50fc6d: 33 c9                        	xor	ecx, ecx
  50fc6f: 75 db                        	jne	0x50fc4c <PHYSFSRWOPS_openAppend+0x10c1bc>
  50fc71: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fc74: 8b 02                        	mov	eax, dword ptr [edx]
  50fc76: 83 b8 b4 00 00 00 00         	cmp	dword ptr [eax + 0xb4], 0x0
  50fc7d: 75 1c                        	jne	0x50fc9b <PHYSFSRWOPS_openAppend+0x10c20b>
  50fc7f: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fc82: 8b 11                        	mov	edx, dword ptr [ecx]
  50fc84: 83 ba bc 00 00 00 00         	cmp	dword ptr [edx + 0xbc], 0x0
  50fc8b: 75 0e                        	jne	0x50fc9b <PHYSFSRWOPS_openAppend+0x10c20b>
  50fc8d: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fc90: 8b 08                        	mov	ecx, dword ptr [eax]
  50fc92: 83 b9 c4 00 00 00 00         	cmp	dword ptr [ecx + 0xc4], 0x0
  50fc99: 74 0f                        	je	0x50fcaa <PHYSFSRWOPS_openAppend+0x10c21a>
  50fc9b: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fc9e: 83 c2 10                     	add	edx, 0x10
  50fca1: 52                           	push	edx
  50fca2: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fca5: e8 96 ee ff ff               	call	0x50eb40 <PHYSFSRWOPS_openAppend+0x10b0b0>
  50fcaa: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fcad: c7 40 08 00 00 00 00         	mov	dword ptr [eax + 0x8], 0x0
  50fcb4: b0 01                        	mov	al, 0x1
  50fcb6: e9 17 09 00 00               	jmp	0x5105d2 <PHYSFSRWOPS_openAppend+0x10cb42>
  50fcbb: 33 c9                        	xor	ecx, ecx
  50fcbd: 0f 85 31 ff ff ff            	jne	0x50fbf4 <PHYSFSRWOPS_openAppend+0x10c164>
  50fcc3: 33 d2                        	xor	edx, edx
  50fcc5: 0f 85 f2 fe ff ff            	jne	0x50fbbd <PHYSFSRWOPS_openAppend+0x10c12d>
  50fccb: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fcce: 8b 08                        	mov	ecx, dword ptr [eax]
  50fcd0: 81 c1 f0 00 00 00            	add	ecx, 0xf0
  50fcd6: e8 05 97 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fcdb: 0f b6 c8                     	movzx	ecx, al
  50fcde: 85 c9                        	test	ecx, ecx
  50fce0: 75 0a                        	jne	0x50fcec <PHYSFSRWOPS_openAppend+0x10c25c>
  50fce2: 6a 01                        	push	0x1
  50fce4: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fce7: e8 f4 eb ff ff               	call	0x50e8e0 <PHYSFSRWOPS_openAppend+0x10ae50>
  50fcec: 33 d2                        	xor	edx, edx
  50fcee: 75 db                        	jne	0x50fccb <PHYSFSRWOPS_openAppend+0x10c23b>
  50fcf0: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fcf3: 8b 08                        	mov	ecx, dword ptr [eax]
  50fcf5: 81 c1 dc 00 00 00            	add	ecx, 0xdc
  50fcfb: e8 e0 96 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50fd00: 0f b6 c8                     	movzx	ecx, al
  50fd03: 85 c9                        	test	ecx, ecx
  50fd05: 75 15                        	jne	0x50fd1c <PHYSFSRWOPS_openAppend+0x10c28c>
  50fd07: 0f b6 15 94 9d d2 00         	movzx	edx, byte ptr [0xd29d94]
  50fd0e: 85 d2                        	test	edx, edx
  50fd10: 75 0a                        	jne	0x50fd1c <PHYSFSRWOPS_openAppend+0x10c28c>
  50fd12: 6a 01                        	push	0x1
  50fd14: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fd17: e8 94 ea ff ff               	call	0x50e7b0 <PHYSFSRWOPS_openAppend+0x10ad20>
  50fd1c: 33 c0                        	xor	eax, eax
  50fd1e: 75 d0                        	jne	0x50fcf0 <PHYSFSRWOPS_openAppend+0x10c260>
  50fd20: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fd23: 8b 11                        	mov	edx, dword ptr [ecx]
  50fd25: 8b 42 34                     	mov	eax, dword ptr [edx + 0x34]
  50fd28: 89 45 e4                     	mov	dword ptr [ebp - 0x1c], eax
  50fd2b: 83 7d e4 02                  	cmp	dword ptr [ebp - 0x1c], 0x2
  50fd2f: 74 1a                        	je	0x50fd4b <PHYSFSRWOPS_openAppend+0x10c2bb>
  50fd31: 83 7d e4 03                  	cmp	dword ptr [ebp - 0x1c], 0x3
  50fd35: 74 62                        	je	0x50fd99 <PHYSFSRWOPS_openAppend+0x10c309>
  50fd37: 83 7d e4 04                  	cmp	dword ptr [ebp - 0x1c], 0x4
  50fd3b: 0f 84 9e 00 00 00            	je	0x50fddf <PHYSFSRWOPS_openAppend+0x10c34f>
  50fd41: e9 24 01 00 00               	jmp	0x50fe6a <PHYSFSRWOPS_openAppend+0x10c3da>
  50fd46: e9 1f 01 00 00               	jmp	0x50fe6a <PHYSFSRWOPS_openAppend+0x10c3da>
  50fd4b: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fd4e: 8b 11                        	mov	edx, dword ptr [ecx]
  50fd50: db 42 38                     	fild	dword ptr [edx + 0x38]
  50fd53: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fd56: 8b 08                        	mov	ecx, dword ptr [eax]
  50fd58: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fd5b: a1 f0 fd ca 00               	mov	eax, dword ptr [0xcafdf0]
  50fd60: 2b 42 08                     	sub	eax, dword ptr [edx + 0x8]
  50fd63: 89 45 dc                     	mov	dword ptr [ebp - 0x24], eax
  50fd66: c7 45 e0 00 00 00 00         	mov	dword ptr [ebp - 0x20], 0x0
  50fd6d: df 6d dc                     	fild	qword ptr [ebp - 0x24]
  50fd70: d8 49 3c                     	fmul	dword ptr [ecx + 0x3c]
  50fd73: de c1                        	faddp	st(1), st
  50fd75: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fd78: d9 59 30                     	fstp	dword ptr [ecx + 0x30]
  50fd7b: 51                           	push	ecx
  50fd7c: d9 05 c4 83 c3 00            	fld	dword ptr [0xc383c4]
  50fd82: d9 1c 24                     	fstp	dword ptr [esp]
  50fd85: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fd88: 83 c2 30                     	add	edx, 0x30
  50fd8b: 52                           	push	edx
  50fd8c: e8 5f 79 4c 00               	call	0x9d76f0 <PHYSFSRWOPS_openAppend+0x5d3c60>
  50fd91: 83 c4 08                     	add	esp, 0x8
  50fd94: e9 d1 00 00 00               	jmp	0x50fe6a <PHYSFSRWOPS_openAppend+0x10c3da>
  50fd99: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fd9c: 8b 08                        	mov	ecx, dword ptr [eax]
  50fd9e: 51                           	push	ecx
  50fd9f: d9 41 3c                     	fld	dword ptr [ecx + 0x3c]
  50fda2: d9 1c 24                     	fstp	dword ptr [esp]
  50fda5: 51                           	push	ecx
  50fda6: d9 ee                        	fldz
  50fda8: d9 1c 24                     	fstp	dword ptr [esp]
  50fdab: b9 00 1a d3 00               	mov	ecx, 0xd31a00
  50fdb0: e8 fb 71 ef ff               	call	0x406fb0 <PHYSFSRWOPS_openAppend+0x3520>
  50fdb5: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fdb8: d8 42 30                     	fadd	dword ptr [edx + 0x30]
  50fdbb: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fdbe: d9 58 30                     	fstp	dword ptr [eax + 0x30]
  50fdc1: 51                           	push	ecx
  50fdc2: d9 05 c4 83 c3 00            	fld	dword ptr [0xc383c4]
  50fdc8: d9 1c 24                     	fstp	dword ptr [esp]
  50fdcb: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fdce: 83 c1 30                     	add	ecx, 0x30
  50fdd1: 51                           	push	ecx
  50fdd2: e8 19 79 4c 00               	call	0x9d76f0 <PHYSFSRWOPS_openAppend+0x5d3c60>
  50fdd7: 83 c4 08                     	add	esp, 0x8
  50fdda: e9 8b 00 00 00               	jmp	0x50fe6a <PHYSFSRWOPS_openAppend+0x10c3da>
  50fddf: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fde2: 8b 02                        	mov	eax, dword ptr [edx]
  50fde4: db 40 38                     	fild	dword ptr [eax + 0x38]
  50fde7: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fdea: 8b 15 f0 fd ca 00            	mov	edx, dword ptr [0xcafdf0]
  50fdf0: 2b 51 08                     	sub	edx, dword ptr [ecx + 0x8]
  50fdf3: 89 55 d4                     	mov	dword ptr [ebp - 0x2c], edx
  50fdf6: c7 45 d8 00 00 00 00         	mov	dword ptr [ebp - 0x28], 0x0
  50fdfd: df 6d d4                     	fild	qword ptr [ebp - 0x2c]
  50fe00: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fe03: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fe06: 8b 50 0c                     	mov	edx, dword ptr [eax + 0xc]
  50fe09: 2b 51 08                     	sub	edx, dword ptr [ecx + 0x8]
  50fe0c: 89 55 cc                     	mov	dword ptr [ebp - 0x34], edx
  50fe0f: c7 45 d0 00 00 00 00         	mov	dword ptr [ebp - 0x30], 0x0
  50fe16: df 6d cc                     	fild	qword ptr [ebp - 0x34]
  50fe19: de f9                        	fdivp	st(1), st
  50fe1b: dc 0d 70 82 c3 00            	fmul	qword ptr [0xc38270]
  50fe21: d9 5d c8                     	fstp	dword ptr [ebp - 0x38]
  50fe24: d9 45 c8                     	fld	dword ptr [ebp - 0x38]
  50fe27: 51                           	push	ecx
  50fe28: d9 1c 24                     	fstp	dword ptr [esp]
  50fe2b: dd 5d c0                     	fstp	qword ptr [ebp - 0x40]
  50fe2e: e8 6d 16 ef ff               	call	0x4014a0 <.text+0x4a0>
  50fe33: 83 c4 04                     	add	esp, 0x4
  50fe36: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fe39: 8b 08                        	mov	ecx, dword ptr [eax]
  50fe3b: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fe3e: 8b 02                        	mov	eax, dword ptr [edx]
  50fe40: db 40 38                     	fild	dword ptr [eax + 0x38]
  50fe43: d8 69 3c                     	fsubr	dword ptr [ecx + 0x3c]
  50fe46: de c9                        	fmulp	st(1), st
  50fe48: dc 45 c0                     	fadd	qword ptr [ebp - 0x40]
  50fe4b: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fe4e: d9 59 30                     	fstp	dword ptr [ecx + 0x30]
  50fe51: 51                           	push	ecx
  50fe52: d9 05 c4 83 c3 00            	fld	dword ptr [0xc383c4]
  50fe58: d9 1c 24                     	fstp	dword ptr [esp]
  50fe5b: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fe5e: 83 c2 30                     	add	edx, 0x30
  50fe61: 52                           	push	edx
  50fe62: e8 89 78 4c 00               	call	0x9d76f0 <PHYSFSRWOPS_openAppend+0x5d3c60>
  50fe67: 83 c4 08                     	add	esp, 0x8
  50fe6a: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fe6d: d9 40 30                     	fld	dword ptr [eax + 0x30]
  50fe70: d9 ee                        	fldz
  50fe72: da e9                        	fucompp
  50fe74: df e0                        	fnstsw	ax
  50fe76: f6 c4 44                     	test	ah, 0x44
  50fe79: 0f 8b 4d 04 00 00            	jnp	0x5102cc <PHYSFSRWOPS_openAppend+0x10c83c>
  50fe7f: 8b 0d 48 0b cf 00            	mov	ecx, dword ptr [0xcf0b48]
  50fe85: 89 4d b8                     	mov	dword ptr [ebp - 0x48], ecx
  50fe88: c7 45 bc 00 00 00 00         	mov	dword ptr [ebp - 0x44], 0x0
  50fe8f: df 6d b8                     	fild	qword ptr [ebp - 0x48]
  50fe92: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fe95: d8 42 34                     	fadd	dword ptr [edx + 0x34]
  50fe98: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fe9b: d9 58 34                     	fstp	dword ptr [eax + 0x34]
  50fe9e: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fea1: d9 41 34                     	fld	dword ptr [ecx + 0x34]
  50fea4: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fea7: d9 42 30                     	fld	dword ptr [edx + 0x30]
  50feaa: de d9                        	fcompp
  50feac: df e0                        	fnstsw	ax
  50feae: f6 c4 41                     	test	ah, 0x41
  50feb1: 0f 8a fb 03 00 00            	jp	0x5102b2 <PHYSFSRWOPS_openAppend+0x10c822>
  50feb7: 8d 4d f8                     	lea	ecx, [ebp - 0x8]
  50feba: e8 b1 9c ef ff               	call	0x409b70 <PHYSFSRWOPS_openAppend+0x60e0>
  50febf: c7 45 f4 00 00 00 00         	mov	dword ptr [ebp - 0xc], 0x0
  50fec6: eb 09                        	jmp	0x50fed1 <PHYSFSRWOPS_openAppend+0x10c441>
  50fec8: 8b 45 f4                     	mov	eax, dword ptr [ebp - 0xc]
  50fecb: 83 c0 01                     	add	eax, 0x1
  50fece: 89 45 f4                     	mov	dword ptr [ebp - 0xc], eax
  50fed1: 83 7d f4 09                  	cmp	dword ptr [ebp - 0xc], 0x9
  50fed5: 0f 8d c0 03 00 00            	jge	0x51029b <PHYSFSRWOPS_openAppend+0x10c80b>
  50fedb: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fede: 83 c1 10                     	add	ecx, 0x10
  50fee1: 51                           	push	ecx
  50fee2: 8d 4d f8                     	lea	ecx, [ebp - 0x8]
  50fee5: e8 76 c1 ef ff               	call	0x40c060 <PHYSFSRWOPS_openAppend+0x85d0>
  50feea: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50feed: 8b 42 78                     	mov	eax, dword ptr [edx + 0x78]
  50fef0: 83 c0 01                     	add	eax, 0x1
  50fef3: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50fef6: 89 41 78                     	mov	dword ptr [ecx + 0x78], eax
  50fef9: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50fefc: 83 c2 18                     	add	edx, 0x18
  50feff: 52                           	push	edx
  50ff00: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50ff03: 83 c0 10                     	add	eax, 0x10
  50ff06: 50                           	push	eax
  50ff07: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50ff0a: 83 c1 38                     	add	ecx, 0x38
  50ff0d: e8 8e 06 f0 ff               	call	0x4105a0 <PHYSFSRWOPS_openAppend+0xcb10>
  50ff12: 0f b6 c8                     	movzx	ecx, al
  50ff15: 85 c9                        	test	ecx, ecx
  50ff17: 75 1d                        	jne	0x50ff36 <PHYSFSRWOPS_openAppend+0x10c4a6>
  50ff19: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50ff1c: 83 c2 10                     	add	edx, 0x10
  50ff1f: 52                           	push	edx
  50ff20: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50ff23: 8b 48 04                     	mov	ecx, dword ptr [eax + 0x4]
  50ff26: e8 b5 4e f4 ff               	call	0x454de0 <PHYSFSRWOPS_openAppend+0x51350>
  50ff2b: 0f b6 c8                     	movzx	ecx, al
  50ff2e: 85 c9                        	test	ecx, ecx
  50ff30: 0f 85 23 01 00 00            	jne	0x510059 <PHYSFSRWOPS_openAppend+0x10c5c9>
  50ff36: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50ff39: 8b 0a                        	mov	ecx, dword ptr [edx]
  50ff3b: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  50ff41: e8 9a 94 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50ff46: 0f b6 c0                     	movzx	eax, al
  50ff49: 85 c0                        	test	eax, eax
  50ff4b: 75 3b                        	jne	0x50ff88 <PHYSFSRWOPS_openAppend+0x10c4f8>
  50ff4d: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50ff50: 83 c1 10                     	add	ecx, 0x10
  50ff53: 51                           	push	ecx
  50ff54: 8d 4d f8                     	lea	ecx, [ebp - 0x8]
  50ff57: e8 84 9e ef ff               	call	0x409de0 <PHYSFSRWOPS_openAppend+0x6350>
  50ff5c: 0f b6 d0                     	movzx	edx, al
  50ff5f: 85 d2                        	test	edx, edx
  50ff61: 74 08                        	je	0x50ff6b <PHYSFSRWOPS_openAppend+0x10c4db>
  50ff63: 8d 45 f8                     	lea	eax, [ebp - 0x8]
  50ff66: 89 45 b4                     	mov	dword ptr [ebp - 0x4c], eax
  50ff69: eb 09                        	jmp	0x50ff74 <PHYSFSRWOPS_openAppend+0x10c4e4>
  50ff6b: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50ff6e: 83 c1 10                     	add	ecx, 0x10
  50ff71: 89 4d b4                     	mov	dword ptr [ebp - 0x4c], ecx
  50ff74: 8b 55 b4                     	mov	edx, dword ptr [ebp - 0x4c]
  50ff77: 89 55 f0                     	mov	dword ptr [ebp - 0x10], edx
  50ff7a: 8b 45 f0                     	mov	eax, dword ptr [ebp - 0x10]
  50ff7d: 50                           	push	eax
  50ff7e: 6a 05                        	push	0x5
  50ff80: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50ff83: e8 38 e0 ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  50ff88: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50ff8b: 8b 09                        	mov	ecx, dword ptr [ecx]
  50ff8d: 81 c1 dc 00 00 00            	add	ecx, 0xdc
  50ff93: e8 48 94 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50ff98: 0f b6 d0                     	movzx	edx, al
  50ff9b: 85 d2                        	test	edx, edx
  50ff9d: 75 15                        	jne	0x50ffb4 <PHYSFSRWOPS_openAppend+0x10c524>
  50ff9f: 0f b6 05 94 9d d2 00         	movzx	eax, byte ptr [0xd29d94]
  50ffa6: 85 c0                        	test	eax, eax
  50ffa8: 75 0a                        	jne	0x50ffb4 <PHYSFSRWOPS_openAppend+0x10c524>
  50ffaa: 6a 02                        	push	0x2
  50ffac: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50ffaf: e8 fc e7 ff ff               	call	0x50e7b0 <PHYSFSRWOPS_openAppend+0x10ad20>
  50ffb4: 33 c9                        	xor	ecx, ecx
  50ffb6: 75 d0                        	jne	0x50ff88 <PHYSFSRWOPS_openAppend+0x10c4f8>
  50ffb8: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50ffbb: 8b 0a                        	mov	ecx, dword ptr [edx]
  50ffbd: 81 c1 f0 00 00 00            	add	ecx, 0xf0
  50ffc3: e8 18 94 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  50ffc8: 0f b6 c0                     	movzx	eax, al
  50ffcb: 85 c0                        	test	eax, eax
  50ffcd: 75 0a                        	jne	0x50ffd9 <PHYSFSRWOPS_openAppend+0x10c549>
  50ffcf: 6a 02                        	push	0x2
  50ffd1: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50ffd4: e8 07 e9 ff ff               	call	0x50e8e0 <PHYSFSRWOPS_openAppend+0x10ae50>
  50ffd9: 33 c9                        	xor	ecx, ecx
  50ffdb: 75 db                        	jne	0x50ffb8 <PHYSFSRWOPS_openAppend+0x10c528>
  50ffdd: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  50ffe0: 8b 02                        	mov	eax, dword ptr [edx]
  50ffe2: 83 b8 b4 00 00 00 00         	cmp	dword ptr [eax + 0xb4], 0x0
  50ffe9: 75 1c                        	jne	0x510007 <PHYSFSRWOPS_openAppend+0x10c577>
  50ffeb: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  50ffee: 8b 11                        	mov	edx, dword ptr [ecx]
  50fff0: 83 ba bc 00 00 00 00         	cmp	dword ptr [edx + 0xbc], 0x0
  50fff7: 75 0e                        	jne	0x510007 <PHYSFSRWOPS_openAppend+0x10c577>
  50fff9: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  50fffc: 8b 08                        	mov	ecx, dword ptr [eax]
  50fffe: 83 b9 c4 00 00 00 00         	cmp	dword ptr [ecx + 0xc4], 0x0
  510005: 74 39                        	je	0x510040 <PHYSFSRWOPS_openAppend+0x10c5b0>
  510007: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  51000a: 83 c2 10                     	add	edx, 0x10
  51000d: 52                           	push	edx
  51000e: 8d 4d f8                     	lea	ecx, [ebp - 0x8]
  510011: e8 ca 9d ef ff               	call	0x409de0 <PHYSFSRWOPS_openAppend+0x6350>
  510016: 0f b6 c0                     	movzx	eax, al
  510019: 85 c0                        	test	eax, eax
  51001b: 74 08                        	je	0x510025 <PHYSFSRWOPS_openAppend+0x10c595>
  51001d: 8d 4d f8                     	lea	ecx, [ebp - 0x8]
  510020: 89 4d b0                     	mov	dword ptr [ebp - 0x50], ecx
  510023: eb 09                        	jmp	0x51002e <PHYSFSRWOPS_openAppend+0x10c59e>
  510025: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510028: 83 c2 10                     	add	edx, 0x10
  51002b: 89 55 b0                     	mov	dword ptr [ebp - 0x50], edx
  51002e: 8b 45 b0                     	mov	eax, dword ptr [ebp - 0x50]
  510031: 89 45 ec                     	mov	dword ptr [ebp - 0x14], eax
  510034: 8b 4d ec                     	mov	ecx, dword ptr [ebp - 0x14]
  510037: 51                           	push	ecx
  510038: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51003b: e8 00 eb ff ff               	call	0x50eb40 <PHYSFSRWOPS_openAppend+0x10b0b0>
  510040: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510043: c7 42 08 00 00 00 00         	mov	dword ptr [edx + 0x8], 0x0
  51004a: b0 01                        	mov	al, 0x1
  51004c: e9 81 05 00 00               	jmp	0x5105d2 <PHYSFSRWOPS_openAppend+0x10cb42>
  510051: 33 c0                        	xor	eax, eax
  510053: 0f 85 dd fe ff ff            	jne	0x50ff36 <PHYSFSRWOPS_openAppend+0x10c4a6>
  510059: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51005c: 83 c1 10                     	add	ecx, 0x10
  51005f: 51                           	push	ecx
  510060: 8d 4d f8                     	lea	ecx, [ebp - 0x8]
  510063: e8 78 9d ef ff               	call	0x409de0 <PHYSFSRWOPS_openAppend+0x6350>
  510068: 0f b6 d0                     	movzx	edx, al
  51006b: 85 d2                        	test	edx, edx
  51006d: 0f 84 23 02 00 00            	je	0x510296 <PHYSFSRWOPS_openAppend+0x10c806>
  510073: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510076: 8b 08                        	mov	ecx, dword ptr [eax]
  510078: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  51007e: e8 5d 93 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  510083: 0f b6 c8                     	movzx	ecx, al
  510086: 85 c9                        	test	ecx, ecx
  510088: 0f 85 e8 00 00 00            	jne	0x510176 <PHYSFSRWOPS_openAppend+0x10c6e6>
  51008e: 8d 55 f8                     	lea	edx, [ebp - 0x8]
  510091: 52                           	push	edx
  510092: 6a 04                        	push	0x4
  510094: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510097: e8 24 df ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  51009c: 0f b6 c0                     	movzx	eax, al
  51009f: 85 c0                        	test	eax, eax
  5100a1: 0f 84 cf 00 00 00            	je	0x510176 <PHYSFSRWOPS_openAppend+0x10c6e6>
  5100a7: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5100aa: 8b 09                        	mov	ecx, dword ptr [ecx]
  5100ac: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  5100b2: e8 29 93 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  5100b7: 0f b6 d0                     	movzx	edx, al
  5100ba: 85 d2                        	test	edx, edx
  5100bc: 75 11                        	jne	0x5100cf <PHYSFSRWOPS_openAppend+0x10c63f>
  5100be: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  5100c1: 83 c0 10                     	add	eax, 0x10
  5100c4: 50                           	push	eax
  5100c5: 6a 05                        	push	0x5
  5100c7: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5100ca: e8 f1 de ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  5100cf: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5100d2: 8b 09                        	mov	ecx, dword ptr [ecx]
  5100d4: 81 c1 dc 00 00 00            	add	ecx, 0xdc
  5100da: e8 01 93 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  5100df: 0f b6 d0                     	movzx	edx, al
  5100e2: 85 d2                        	test	edx, edx
  5100e4: 75 15                        	jne	0x5100fb <PHYSFSRWOPS_openAppend+0x10c66b>
  5100e6: 0f b6 05 94 9d d2 00         	movzx	eax, byte ptr [0xd29d94]
  5100ed: 85 c0                        	test	eax, eax
  5100ef: 75 0a                        	jne	0x5100fb <PHYSFSRWOPS_openAppend+0x10c66b>
  5100f1: 6a 02                        	push	0x2
  5100f3: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5100f6: e8 b5 e6 ff ff               	call	0x50e7b0 <PHYSFSRWOPS_openAppend+0x10ad20>
  5100fb: 33 c9                        	xor	ecx, ecx
  5100fd: 75 d0                        	jne	0x5100cf <PHYSFSRWOPS_openAppend+0x10c63f>
  5100ff: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510102: 8b 0a                        	mov	ecx, dword ptr [edx]
  510104: 81 c1 f0 00 00 00            	add	ecx, 0xf0
  51010a: e8 d1 92 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  51010f: 0f b6 c0                     	movzx	eax, al
  510112: 85 c0                        	test	eax, eax
  510114: 75 0a                        	jne	0x510120 <PHYSFSRWOPS_openAppend+0x10c690>
  510116: 6a 02                        	push	0x2
  510118: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51011b: e8 c0 e7 ff ff               	call	0x50e8e0 <PHYSFSRWOPS_openAppend+0x10ae50>
  510120: 33 c9                        	xor	ecx, ecx
  510122: 75 db                        	jne	0x5100ff <PHYSFSRWOPS_openAppend+0x10c66f>
  510124: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510127: 8b 02                        	mov	eax, dword ptr [edx]
  510129: 83 b8 b4 00 00 00 00         	cmp	dword ptr [eax + 0xb4], 0x0
  510130: 75 1c                        	jne	0x51014e <PHYSFSRWOPS_openAppend+0x10c6be>
  510132: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510135: 8b 11                        	mov	edx, dword ptr [ecx]
  510137: 83 ba bc 00 00 00 00         	cmp	dword ptr [edx + 0xbc], 0x0
  51013e: 75 0e                        	jne	0x51014e <PHYSFSRWOPS_openAppend+0x10c6be>
  510140: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510143: 8b 08                        	mov	ecx, dword ptr [eax]
  510145: 83 b9 c4 00 00 00 00         	cmp	dword ptr [ecx + 0xc4], 0x0
  51014c: 74 0f                        	je	0x51015d <PHYSFSRWOPS_openAppend+0x10c6cd>
  51014e: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510151: 83 c2 10                     	add	edx, 0x10
  510154: 52                           	push	edx
  510155: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510158: e8 e3 e9 ff ff               	call	0x50eb40 <PHYSFSRWOPS_openAppend+0x10b0b0>
  51015d: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510160: c7 40 08 00 00 00 00         	mov	dword ptr [eax + 0x8], 0x0
  510167: b0 01                        	mov	al, 0x1
  510169: e9 64 04 00 00               	jmp	0x5105d2 <PHYSFSRWOPS_openAppend+0x10cb42>
  51016e: 33 c9                        	xor	ecx, ecx
  510170: 0f 85 31 ff ff ff            	jne	0x5100a7 <PHYSFSRWOPS_openAppend+0x10c617>
  510176: 33 d2                        	xor	edx, edx
  510178: 0f 85 f5 fe ff ff            	jne	0x510073 <PHYSFSRWOPS_openAppend+0x10c5e3>
  51017e: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510181: 8b 08                        	mov	ecx, dword ptr [eax]
  510183: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  510189: e8 52 92 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  51018e: 0f b6 c8                     	movzx	ecx, al
  510191: 85 c9                        	test	ecx, ecx
  510193: 0f 85 eb 00 00 00            	jne	0x510284 <PHYSFSRWOPS_openAppend+0x10c7f4>
  510199: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  51019c: 83 c2 10                     	add	edx, 0x10
  51019f: 52                           	push	edx
  5101a0: 6a 03                        	push	0x3
  5101a2: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5101a5: e8 16 de ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  5101aa: 0f b6 c0                     	movzx	eax, al
  5101ad: 85 c0                        	test	eax, eax
  5101af: 0f 84 cf 00 00 00            	je	0x510284 <PHYSFSRWOPS_openAppend+0x10c7f4>
  5101b5: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5101b8: 8b 09                        	mov	ecx, dword ptr [ecx]
  5101ba: 81 c1 cc 00 00 00            	add	ecx, 0xcc
  5101c0: e8 1b 92 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  5101c5: 0f b6 d0                     	movzx	edx, al
  5101c8: 85 d2                        	test	edx, edx
  5101ca: 75 11                        	jne	0x5101dd <PHYSFSRWOPS_openAppend+0x10c74d>
  5101cc: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  5101cf: 83 c0 10                     	add	eax, 0x10
  5101d2: 50                           	push	eax
  5101d3: 6a 05                        	push	0x5
  5101d5: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5101d8: e8 e3 dd ff ff               	call	0x50dfc0 <PHYSFSRWOPS_openAppend+0x10a530>
  5101dd: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5101e0: 8b 09                        	mov	ecx, dword ptr [ecx]
  5101e2: 81 c1 dc 00 00 00            	add	ecx, 0xdc
  5101e8: e8 f3 91 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  5101ed: 0f b6 d0                     	movzx	edx, al
  5101f0: 85 d2                        	test	edx, edx
  5101f2: 75 15                        	jne	0x510209 <PHYSFSRWOPS_openAppend+0x10c779>
  5101f4: 0f b6 05 94 9d d2 00         	movzx	eax, byte ptr [0xd29d94]
  5101fb: 85 c0                        	test	eax, eax
  5101fd: 75 0a                        	jne	0x510209 <PHYSFSRWOPS_openAppend+0x10c779>
  5101ff: 6a 02                        	push	0x2
  510201: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510204: e8 a7 e5 ff ff               	call	0x50e7b0 <PHYSFSRWOPS_openAppend+0x10ad20>
  510209: 33 c9                        	xor	ecx, ecx
  51020b: 75 d0                        	jne	0x5101dd <PHYSFSRWOPS_openAppend+0x10c74d>
  51020d: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510210: 8b 0a                        	mov	ecx, dword ptr [edx]
  510212: 81 c1 f0 00 00 00            	add	ecx, 0xf0
  510218: e8 c3 91 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  51021d: 0f b6 c0                     	movzx	eax, al
  510220: 85 c0                        	test	eax, eax
  510222: 75 0a                        	jne	0x51022e <PHYSFSRWOPS_openAppend+0x10c79e>
  510224: 6a 02                        	push	0x2
  510226: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510229: e8 b2 e6 ff ff               	call	0x50e8e0 <PHYSFSRWOPS_openAppend+0x10ae50>
  51022e: 33 c9                        	xor	ecx, ecx
  510230: 75 db                        	jne	0x51020d <PHYSFSRWOPS_openAppend+0x10c77d>
  510232: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510235: 8b 02                        	mov	eax, dword ptr [edx]
  510237: 83 b8 b4 00 00 00 00         	cmp	dword ptr [eax + 0xb4], 0x0
  51023e: 75 1c                        	jne	0x51025c <PHYSFSRWOPS_openAppend+0x10c7cc>
  510240: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510243: 8b 11                        	mov	edx, dword ptr [ecx]
  510245: 83 ba bc 00 00 00 00         	cmp	dword ptr [edx + 0xbc], 0x0
  51024c: 75 0e                        	jne	0x51025c <PHYSFSRWOPS_openAppend+0x10c7cc>
  51024e: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510251: 8b 08                        	mov	ecx, dword ptr [eax]
  510253: 83 b9 c4 00 00 00 00         	cmp	dword ptr [ecx + 0xc4], 0x0
  51025a: 74 0f                        	je	0x51026b <PHYSFSRWOPS_openAppend+0x10c7db>
  51025c: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  51025f: 83 c2 10                     	add	edx, 0x10
  510262: 52                           	push	edx
  510263: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510266: e8 d5 e8 ff ff               	call	0x50eb40 <PHYSFSRWOPS_openAppend+0x10b0b0>
  51026b: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  51026e: c7 40 08 00 00 00 00         	mov	dword ptr [eax + 0x8], 0x0
  510275: b0 01                        	mov	al, 0x1
  510277: e9 56 03 00 00               	jmp	0x5105d2 <PHYSFSRWOPS_openAppend+0x10cb42>
  51027c: 33 c9                        	xor	ecx, ecx
  51027e: 0f 85 31 ff ff ff            	jne	0x5101b5 <PHYSFSRWOPS_openAppend+0x10c725>
  510284: 33 d2                        	xor	edx, edx
  510286: 0f 85 f2 fe ff ff            	jne	0x51017e <PHYSFSRWOPS_openAppend+0x10c6ee>
  51028c: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  51028f: c7 40 78 01 00 00 00         	mov	dword ptr [eax + 0x78], 0x1
  510296: e9 2d fc ff ff               	jmp	0x50fec8 <PHYSFSRWOPS_openAppend+0x10c438>
  51029b: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51029e: d9 41 34                     	fld	dword ptr [ecx + 0x34]
  5102a1: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  5102a4: d8 62 30                     	fsub	dword ptr [edx + 0x30]
  5102a7: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  5102aa: d9 58 34                     	fstp	dword ptr [eax + 0x34]
  5102ad: e9 ec fb ff ff               	jmp	0x50fe9e <PHYSFSRWOPS_openAppend+0x10c40e>
  5102b2: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5102b5: d9 41 34                     	fld	dword ptr [ecx + 0x34]
  5102b8: dc 1d b8 80 c3 00            	fcomp	qword ptr [0xc380b8]
  5102be: df e0                        	fnstsw	ax
  5102c0: f6 c4 05                     	test	ah, 0x5
  5102c3: 7a 07                        	jp	0x5102cc <PHYSFSRWOPS_openAppend+0x10c83c>
  5102c5: 32 c0                        	xor	al, al
  5102c7: e9 06 03 00 00               	jmp	0x5105d2 <PHYSFSRWOPS_openAppend+0x10cb42>
  5102cc: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  5102cf: 8b 02                        	mov	eax, dword ptr [edx]
  5102d1: 83 78 48 0f                  	cmp	dword ptr [eax + 0x48], 0xf
  5102d5: 0f 8c f5 02 00 00            	jl	0x5105d0 <PHYSFSRWOPS_openAppend+0x10cb40>
  5102db: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5102de: 8b 11                        	mov	edx, dword ptr [ecx]
  5102e0: 8b 42 48                     	mov	eax, dword ptr [edx + 0x48]
  5102e3: 89 45 ac                     	mov	dword ptr [ebp - 0x54], eax
  5102e6: 8b 4d ac                     	mov	ecx, dword ptr [ebp - 0x54]
  5102e9: 83 e9 0f                     	sub	ecx, 0xf
  5102ec: 89 4d ac                     	mov	dword ptr [ebp - 0x54], ecx
  5102ef: 83 7d ac 0c                  	cmp	dword ptr [ebp - 0x54], 0xc
  5102f3: 0f 87 f4 00 00 00            	ja	0x5103ed <PHYSFSRWOPS_openAppend+0x10c95d>
  5102f9: 8b 55 ac                     	mov	edx, dword ptr [ebp - 0x54]
  5102fc: 0f b6 82 e8 05 51 00         	movzx	eax, byte ptr [edx + 0x5105e8]
  510303: ff 24 85 d8 05 51 00         	jmp	dword ptr [4*eax + 0x5105d8]
  51030a: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51030d: 83 c1 10                     	add	ecx, 0x10
  510310: 51                           	push	ecx
  510311: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510314: 8b 02                        	mov	eax, dword ptr [edx]
  510316: 8b 48 48                     	mov	ecx, dword ptr [eax + 0x48]
  510319: 83 e9 0f                     	sub	ecx, 0xf
  51031c: 51                           	push	ecx
  51031d: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510320: 8b 42 04                     	mov	eax, dword ptr [edx + 0x4]
  510323: 8b 08                        	mov	ecx, dword ptr [eax]
  510325: 83 c1 6c                     	add	ecx, 0x6c
  510328: e8 03 87 4a 00               	call	0x9b8a30 <PHYSFSRWOPS_openAppend+0x5b4fa0>
  51032d: 8b 08                        	mov	ecx, dword ptr [eax]
  51032f: e8 3c f8 4b 00               	call	0x9cfb70 <PHYSFSRWOPS_openAppend+0x5cc0e0>
  510334: 8b c8                        	mov	ecx, eax
  510336: e8 e5 c4 f4 ff               	call	0x45c820 <PHYSFSRWOPS_openAppend+0x58d90>
  51033b: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51033e: 89 41 7c                     	mov	dword ptr [ecx + 0x7c], eax
  510341: e9 8a 02 00 00               	jmp	0x5105d0 <PHYSFSRWOPS_openAppend+0x10cb40>
  510346: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510349: a1 f0 fd ca 00               	mov	eax, dword ptr [0xcafdf0]
  51034e: 2b 42 08                     	sub	eax, dword ptr [edx + 0x8]
  510351: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510354: 8b 09                        	mov	ecx, dword ptr [ecx]
  510356: 33 d2                        	xor	edx, edx
  510358: f7 71 5c                     	div	dword ptr [ecx + 0x5c]
  51035b: 8b f0                        	mov	esi, eax
  51035d: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510360: 8b 0a                        	mov	ecx, dword ptr [edx]
  510362: 83 c1 4c                     	add	ecx, 0x4c
  510365: e8 b6 b5 4a 00               	call	0x9bb920 <PHYSFSRWOPS_openAppend+0x5b7e90>
  51036a: 8b c8                        	mov	ecx, eax
  51036c: 8b c6                        	mov	eax, esi
  51036e: 33 d2                        	xor	edx, edx
  510370: f7 f1                        	div	ecx
  510372: 52                           	push	edx
  510373: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510376: 8b 0a                        	mov	ecx, dword ptr [edx]
  510378: 83 c1 4c                     	add	ecx, 0x4c
  51037b: e8 b0 86 4a 00               	call	0x9b8a30 <PHYSFSRWOPS_openAppend+0x5b4fa0>
  510380: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510383: 8b 10                        	mov	edx, dword ptr [eax]
  510385: 89 51 7c                     	mov	dword ptr [ecx + 0x7c], edx
  510388: e9 43 02 00 00               	jmp	0x5105d0 <PHYSFSRWOPS_openAppend+0x10cb40>
  51038d: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510390: 83 c0 20                     	add	eax, 0x20
  510393: 50                           	push	eax
  510394: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510397: a1 f0 fd ca 00               	mov	eax, dword ptr [0xcafdf0]
  51039c: 2b 41 08                     	sub	eax, dword ptr [ecx + 0x8]
  51039f: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  5103a2: 8b 0a                        	mov	ecx, dword ptr [edx]
  5103a4: 33 d2                        	xor	edx, edx
  5103a6: f7 71 5c                     	div	dword ptr [ecx + 0x5c]
  5103a9: 8b f0                        	mov	esi, eax
  5103ab: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  5103ae: 8b 42 04                     	mov	eax, dword ptr [edx + 0x4]
  5103b1: 8b 08                        	mov	ecx, dword ptr [eax]
  5103b3: 83 c1 6c                     	add	ecx, 0x6c
  5103b6: e8 65 b5 4a 00               	call	0x9bb920 <PHYSFSRWOPS_openAppend+0x5b7e90>
  5103bb: 8b c8                        	mov	ecx, eax
  5103bd: 8b c6                        	mov	eax, esi
  5103bf: 33 d2                        	xor	edx, edx
  5103c1: f7 f1                        	div	ecx
  5103c3: 52                           	push	edx
  5103c4: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  5103c7: 8b 42 04                     	mov	eax, dword ptr [edx + 0x4]
  5103ca: 8b 08                        	mov	ecx, dword ptr [eax]
  5103cc: 83 c1 6c                     	add	ecx, 0x6c
  5103cf: e8 5c 86 4a 00               	call	0x9b8a30 <PHYSFSRWOPS_openAppend+0x5b4fa0>
  5103d4: 8b 08                        	mov	ecx, dword ptr [eax]
  5103d6: e8 95 f7 4b 00               	call	0x9cfb70 <PHYSFSRWOPS_openAppend+0x5cc0e0>
  5103db: 8b c8                        	mov	ecx, eax
  5103dd: e8 3e c4 f4 ff               	call	0x45c820 <PHYSFSRWOPS_openAppend+0x58d90>
  5103e2: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5103e5: 89 41 7c                     	mov	dword ptr [ecx + 0x7c], eax
  5103e8: e9 e3 01 00 00               	jmp	0x5105d0 <PHYSFSRWOPS_openAppend+0x10cb40>
  5103ed: 8b 15 48 0b cf 00            	mov	edx, dword ptr [0xcf0b48]
  5103f3: 89 55 a4                     	mov	dword ptr [ebp - 0x5c], edx
  5103f6: c7 45 a8 00 00 00 00         	mov	dword ptr [ebp - 0x58], 0x0
  5103fd: df 6d a4                     	fild	qword ptr [ebp - 0x5c]
  510400: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510403: 8b 88 80 00 00 00            	mov	ecx, dword ptr [eax + 0x80]
  510409: 89 4d 9c                     	mov	dword ptr [ebp - 0x64], ecx
  51040c: c7 45 a0 00 00 00 00         	mov	dword ptr [ebp - 0x60], 0x0
  510413: df 6d 9c                     	fild	qword ptr [ebp - 0x64]
  510416: de c1                        	faddp	st(1), st
  510418: d9 7d 9a                     	fnstcw	word ptr [ebp - 0x66]
  51041b: 0f b7 45 9a                  	movzx	eax, word ptr [ebp - 0x66]
  51041f: 0d 00 0c 00 00               	or	eax, 0xc00
  510424: 89 45 94                     	mov	dword ptr [ebp - 0x6c], eax
  510427: d9 6d 94                     	fldcw	word ptr [ebp - 0x6c]
  51042a: df 7d 8c                     	fistp	qword ptr [ebp - 0x74]
  51042d: d9 6d 9a                     	fldcw	word ptr [ebp - 0x66]
  510430: 8b 55 8c                     	mov	edx, dword ptr [ebp - 0x74]
  510433: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510436: 89 90 80 00 00 00            	mov	dword ptr [eax + 0x80], edx
  51043c: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51043f: 8b 11                        	mov	edx, dword ptr [ecx]
  510441: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510444: 8b 88 80 00 00 00            	mov	ecx, dword ptr [eax + 0x80]
  51044a: 3b 4a 5c                     	cmp	ecx, dword ptr [edx + 0x5c]
  51044d: 0f 82 4e 01 00 00            	jb	0x5105a1 <PHYSFSRWOPS_openAppend+0x10cb11>
  510453: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510456: 8b 02                        	mov	eax, dword ptr [edx]
  510458: 8b 48 48                     	mov	ecx, dword ptr [eax + 0x48]
  51045b: 89 4d 88                     	mov	dword ptr [ebp - 0x78], ecx
  51045e: 8b 55 88                     	mov	edx, dword ptr [ebp - 0x78]
  510461: 83 ea 14                     	sub	edx, 0x14
  510464: 89 55 88                     	mov	dword ptr [ebp - 0x78], edx
  510467: 83 7d 88 08                  	cmp	dword ptr [ebp - 0x78], 0x8
  51046b: 0f 87 16 01 00 00            	ja	0x510587 <PHYSFSRWOPS_openAppend+0x10caf7>
  510471: 8b 45 88                     	mov	eax, dword ptr [ebp - 0x78]
  510474: ff 24 85 f8 05 51 00         	jmp	dword ptr [4*eax + 0x5105f8]
  51047b: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51047e: 8b 11                        	mov	edx, dword ptr [ecx]
  510480: 83 c2 4c                     	add	edx, 0x4c
  510483: 52                           	push	edx
  510484: e8 c7 cf 4c 00               	call	0x9dd450 <PHYSFSRWOPS_openAppend+0x5d99c0>
  510489: 83 c4 04                     	add	esp, 0x4
  51048c: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51048f: 89 41 7c                     	mov	dword ptr [ecx + 0x7c], eax
  510492: e9 f0 00 00 00               	jmp	0x510587 <PHYSFSRWOPS_openAppend+0x10caf7>
  510497: 68 64 57 d3 00               	push	0xd35764
  51049c: e8 ef 82 ef ff               	call	0x408790 <PHYSFSRWOPS_openAppend+0x4d00>
  5104a1: 83 c4 04                     	add	esp, 0x4
  5104a4: 0f be d0                     	movsx	edx, al
  5104a7: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  5104aa: 89 50 7c                     	mov	dword ptr [eax + 0x7c], edx
  5104ad: e9 d5 00 00 00               	jmp	0x510587 <PHYSFSRWOPS_openAppend+0x10caf7>
  5104b2: 6a 0a                        	push	0xa
  5104b4: 68 00 a4 bb 00               	push	0xbba400
  5104b9: e8 62 b3 4c 00               	call	0x9db820 <PHYSFSRWOPS_openAppend+0x5d7d90>
  5104be: 83 c4 08                     	add	esp, 0x8
  5104c1: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5104c4: 89 41 7c                     	mov	dword ptr [ecx + 0x7c], eax
  5104c7: e9 bb 00 00 00               	jmp	0x510587 <PHYSFSRWOPS_openAppend+0x10caf7>
  5104cc: 68 f4 56 d3 00               	push	0xd356f4
  5104d1: e8 ba 82 ef ff               	call	0x408790 <PHYSFSRWOPS_openAppend+0x4d00>
  5104d6: 83 c4 04                     	add	esp, 0x4
  5104d9: 0f be d0                     	movsx	edx, al
  5104dc: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  5104df: 89 50 7c                     	mov	dword ptr [eax + 0x7c], edx
  5104e2: e9 a0 00 00 00               	jmp	0x510587 <PHYSFSRWOPS_openAppend+0x10caf7>
  5104e7: 68 f4 56 d3 00               	push	0xd356f4
  5104ec: e8 9f 82 ef ff               	call	0x408790 <PHYSFSRWOPS_openAppend+0x4d00>
  5104f1: 83 c4 04                     	add	esp, 0x4
  5104f4: 0f be c8                     	movsx	ecx, al
  5104f7: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  5104fa: 89 4a 7c                     	mov	dword ptr [edx + 0x7c], ecx
  5104fd: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510500: 8b 48 7c                     	mov	ecx, dword ptr [eax + 0x7c]
  510503: 51                           	push	ecx
  510504: ff 15 44 23 b6 00            	call	dword ptr [0xb62344]
  51050a: 83 c4 04                     	add	esp, 0x4
  51050d: 85 c0                        	test	eax, eax
  51050f: 74 16                        	je	0x510527 <PHYSFSRWOPS_openAppend+0x10ca97>
  510511: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  510514: 8b 42 7c                     	mov	eax, dword ptr [edx + 0x7c]
  510517: 50                           	push	eax
  510518: ff 15 4c 23 b6 00            	call	dword ptr [0xb6234c]
  51051e: 83 c4 04                     	add	esp, 0x4
  510521: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510524: 89 41 7c                     	mov	dword ptr [ecx + 0x7c], eax
  510527: eb 5e                        	jmp	0x510587 <PHYSFSRWOPS_openAppend+0x10caf7>
  510529: 68 ac 5a d3 00               	push	0xd35aac
  51052e: e8 5d 82 ef ff               	call	0x408790 <PHYSFSRWOPS_openAppend+0x4d00>
  510533: 83 c4 04                     	add	esp, 0x4
  510536: 0f be d0                     	movsx	edx, al
  510539: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  51053c: 89 50 7c                     	mov	dword ptr [eax + 0x7c], edx
  51053f: eb 46                        	jmp	0x510587 <PHYSFSRWOPS_openAppend+0x10caf7>
  510541: 6a 0e                        	push	0xe
  510543: 68 28 a4 bb 00               	push	0xbba428
  510548: e8 d3 b2 4c 00               	call	0x9db820 <PHYSFSRWOPS_openAppend+0x5d7d90>
  51054d: 83 c4 08                     	add	esp, 0x8
  510550: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510553: 89 41 7c                     	mov	dword ptr [ecx + 0x7c], eax
  510556: eb 2f                        	jmp	0x510587 <PHYSFSRWOPS_openAppend+0x10caf7>
  510558: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  51055b: 83 c2 20                     	add	edx, 0x20
  51055e: 52                           	push	edx
  51055f: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  510562: 8b 48 04                     	mov	ecx, dword ptr [eax + 0x4]
  510565: 8b 11                        	mov	edx, dword ptr [ecx]
  510567: 83 c2 6c                     	add	edx, 0x6c
  51056a: 52                           	push	edx
  51056b: e8 e0 ce 4c 00               	call	0x9dd450 <PHYSFSRWOPS_openAppend+0x5d99c0>
  510570: 83 c4 04                     	add	esp, 0x4
  510573: 8b c8                        	mov	ecx, eax
  510575: e8 f6 f5 4b 00               	call	0x9cfb70 <PHYSFSRWOPS_openAppend+0x5cc0e0>
  51057a: 8b c8                        	mov	ecx, eax
  51057c: e8 9f c2 f4 ff               	call	0x45c820 <PHYSFSRWOPS_openAppend+0x58d90>
  510581: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  510584: 89 41 7c                     	mov	dword ptr [ecx + 0x7c], eax
  510587: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  51058a: 8b 02                        	mov	eax, dword ptr [edx]
  51058c: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  51058f: 8b 91 80 00 00 00            	mov	edx, dword ptr [ecx + 0x80]
  510595: 2b 50 5c                     	sub	edx, dword ptr [eax + 0x5c]
  510598: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  51059b: 89 90 80 00 00 00            	mov	dword ptr [eax + 0x80], edx
  5105a1: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5105a4: 8b 11                        	mov	edx, dword ptr [ecx]
  5105a6: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  5105a9: 8b 88 80 00 00 00            	mov	ecx, dword ptr [eax + 0x80]
  5105af: 3b 4a 5c                     	cmp	ecx, dword ptr [edx + 0x5c]
  5105b2: 72 1c                        	jb	0x5105d0 <PHYSFSRWOPS_openAppend+0x10cb40>
  5105b4: 8b 55 e8                     	mov	edx, dword ptr [ebp - 0x18]
  5105b7: 8b 02                        	mov	eax, dword ptr [edx]
  5105b9: 8b 4d e8                     	mov	ecx, dword ptr [ebp - 0x18]
  5105bc: 8b 91 80 00 00 00            	mov	edx, dword ptr [ecx + 0x80]
  5105c2: 2b 50 5c                     	sub	edx, dword ptr [eax + 0x5c]
  5105c5: 8b 45 e8                     	mov	eax, dword ptr [ebp - 0x18]
  5105c8: 89 90 80 00 00 00            	mov	dword ptr [eax + 0x80], edx
  5105ce: eb d1                        	jmp	0x5105a1 <PHYSFSRWOPS_openAppend+0x10cb11>
  5105d0: 32 c0                        	xor	al, al
  5105d2: 5e                           	pop	esi
  5105d3: 8b e5                        	mov	esp, ebp

; action construction and active/deadline fields

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  50ef00: 55                           	push	ebp
  50ef01: 8b ec                        	mov	ebp, esp
  50ef03: 81 ec 80 00 00 00            	sub	esp, 0x80
  50ef09: 56                           	push	esi
  50ef0a: 89 4d e4                     	mov	dword ptr [ebp - 0x1c], ecx
  50ef0d: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50ef10: 8b 4d 0c                     	mov	ecx, dword ptr [ebp + 0xc]
  50ef13: 89 08                        	mov	dword ptr [eax], ecx
  50ef15: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50ef18: 8b 45 08                     	mov	eax, dword ptr [ebp + 0x8]
  50ef1b: 89 42 04                     	mov	dword ptr [edx + 0x4], eax
  50ef1e: 8b 4d 10                     	mov	ecx, dword ptr [ebp + 0x10]
  50ef21: 51                           	push	ecx
  50ef22: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50ef25: 83 c1 10                     	add	ecx, 0x10
  50ef28: e8 33 d1 ef ff               	call	0x40c060 <PHYSFSRWOPS_openAppend+0x85d0>
  50ef2d: 8b 55 14                     	mov	edx, dword ptr [ebp + 0x14]
  50ef30: 52                           	push	edx
  50ef31: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50ef34: 83 c1 18                     	add	ecx, 0x18
  50ef37: e8 24 d1 ef ff               	call	0x40c060 <PHYSFSRWOPS_openAppend+0x85d0>
  50ef3c: 8b 45 10                     	mov	eax, dword ptr [ebp + 0x10]
  50ef3f: 50                           	push	eax
  50ef40: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50ef43: 83 c1 20                     	add	ecx, 0x20
  50ef46: e8 15 d1 ef ff               	call	0x40c060 <PHYSFSRWOPS_openAppend+0x85d0>
  50ef4b: 8b 4d 14                     	mov	ecx, dword ptr [ebp + 0x14]
  50ef4e: 51                           	push	ecx
  50ef4f: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50ef52: 83 c1 28                     	add	ecx, 0x28
  50ef55: e8 06 d1 ef ff               	call	0x40c060 <PHYSFSRWOPS_openAppend+0x85d0>
  50ef5a: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50ef5d: d9 ee                        	fldz
  50ef5f: d9 5a 34                     	fstp	dword ptr [edx + 0x34]
  50ef62: 83 7d 18 00                  	cmp	dword ptr [ebp + 0x18], 0x0
  50ef66: 0f 84 fc 00 00 00            	je	0x50f068 <PHYSFSRWOPS_openAppend+0x10b5d8>
  50ef6c: 8b 45 18                     	mov	eax, dword ptr [ebp + 0x18]
  50ef6f: 50                           	push	eax
  50ef70: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50ef73: 83 c1 68                     	add	ecx, 0x68
  50ef76: e8 e5 d0 ef ff               	call	0x40c060 <PHYSFSRWOPS_openAppend+0x85d0>
  50ef7b: 8b 4d 1c                     	mov	ecx, dword ptr [ebp + 0x1c]
  50ef7e: 51                           	push	ecx
  50ef7f: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50ef82: 83 c1 70                     	add	ecx, 0x70
  50ef85: e8 d6 d0 ef ff               	call	0x40c060 <PHYSFSRWOPS_openAppend+0x85d0>
  50ef8a: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50ef8d: 8b 02                        	mov	eax, dword ptr [edx]
  50ef8f: 8b 48 34                     	mov	ecx, dword ptr [eax + 0x34]
  50ef92: 89 4d e0                     	mov	dword ptr [ebp - 0x20], ecx
  50ef95: 83 7d e0 04                  	cmp	dword ptr [ebp - 0x20], 0x4
  50ef99: 0f 87 c7 00 00 00            	ja	0x50f066 <PHYSFSRWOPS_openAppend+0x10b5d6>
  50ef9f: 8b 55 e0                     	mov	edx, dword ptr [ebp - 0x20]
  50efa2: ff 24 95 ac f8 50 00         	jmp	dword ptr [4*edx + 0x50f8ac]
  50efa9: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50efac: d9 ee                        	fldz
  50efae: d9 58 30                     	fstp	dword ptr [eax + 0x30]
  50efb1: e9 b0 00 00 00               	jmp	0x50f066 <PHYSFSRWOPS_openAppend+0x10b5d6>
  50efb6: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50efb9: 8b 11                        	mov	edx, dword ptr [ecx]
  50efbb: d9 42 3c                     	fld	dword ptr [edx + 0x3c]
  50efbe: dc 1d b8 80 c3 00            	fcomp	qword ptr [0xc380b8]
  50efc4: df e0                        	fnstsw	ax
  50efc6: f6 c4 41                     	test	ah, 0x41
  50efc9: 75 31                        	jne	0x50effc <PHYSFSRWOPS_openAppend+0x10b56c>
  50efcb: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50efce: 8b 30                        	mov	esi, dword ptr [eax]
  50efd0: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50efd3: 8b 11                        	mov	edx, dword ptr [ecx]
  50efd5: 51                           	push	ecx
  50efd6: d9 42 3c                     	fld	dword ptr [edx + 0x3c]
  50efd9: d9 1c 24                     	fstp	dword ptr [esp]
  50efdc: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50efdf: 8b 08                        	mov	ecx, dword ptr [eax]
  50efe1: d9 41 3c                     	fld	dword ptr [ecx + 0x3c]
  50efe4: d9 e0                        	fchs
  50efe6: 51                           	push	ecx
  50efe7: d9 1c 24                     	fstp	dword ptr [esp]
  50efea: b9 00 1a d3 00               	mov	ecx, 0xd31a00
  50efef: e8 0c 7f ef ff               	call	0x406f00 <PHYSFSRWOPS_openAppend+0x3470>
  50eff4: 03 46 38                     	add	eax, dword ptr [esi + 0x38]
  50eff7: 89 45 dc                     	mov	dword ptr [ebp - 0x24], eax
  50effa: eb 0b                        	jmp	0x50f007 <PHYSFSRWOPS_openAppend+0x10b577>
  50effc: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50efff: 8b 02                        	mov	eax, dword ptr [edx]
  50f001: 8b 48 38                     	mov	ecx, dword ptr [eax + 0x38]
  50f004: 89 4d dc                     	mov	dword ptr [ebp - 0x24], ecx
  50f007: db 45 dc                     	fild	dword ptr [ebp - 0x24]
  50f00a: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50f00d: d9 5a 30                     	fstp	dword ptr [edx + 0x30]
  50f010: eb 10                        	jmp	0x50f022 <PHYSFSRWOPS_openAppend+0x10b592>
  50f012: eb 0e                        	jmp	0x50f022 <PHYSFSRWOPS_openAppend+0x10b592>
  50f014: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f017: 8b 08                        	mov	ecx, dword ptr [eax]
  50f019: db 41 38                     	fild	dword ptr [ecx + 0x38]
  50f01c: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50f01f: d9 5a 30                     	fstp	dword ptr [edx + 0x30]
  50f022: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f025: 8b 08                        	mov	ecx, dword ptr [eax]
  50f027: 83 c1 40                     	add	ecx, 0x40
  50f02a: e8 81 d2 ef ff               	call	0x40c2b0 <PHYSFSRWOPS_openAppend+0x8820>
  50f02f: f7 d8                        	neg	eax
  50f031: 89 45 d8                     	mov	dword ptr [ebp - 0x28], eax
  50f034: db 45 d8                     	fild	dword ptr [ebp - 0x28]
  50f037: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50f03a: d9 59 34                     	fstp	dword ptr [ecx + 0x34]
  50f03d: 6a 09                        	push	0x9
  50f03f: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50f042: 83 c2 70                     	add	edx, 0x70
  50f045: 52                           	push	edx
  50f046: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f049: 83 c0 68                     	add	eax, 0x68
  50f04c: 50                           	push	eax
  50f04d: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50f050: 83 c1 28                     	add	ecx, 0x28
  50f053: 51                           	push	ecx
  50f054: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50f057: 83 c2 20                     	add	edx, 0x20
  50f05a: 52                           	push	edx
  50f05b: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50f05e: 83 c1 38                     	add	ecx, 0x38
  50f061: e8 ca 14 f0 ff               	call	0x410530 <PHYSFSRWOPS_openAppend+0xcaa0>
  50f066: eb 08                        	jmp	0x50f070 <PHYSFSRWOPS_openAppend+0x10b5e0>
  50f068: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f06b: d9 ee                        	fldz
  50f06d: d9 58 30                     	fstp	dword ptr [eax + 0x30]
  50f070: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50f073: 8b 55 20                     	mov	edx, dword ptr [ebp + 0x20]
  50f076: 89 51 78                     	mov	dword ptr [ecx + 0x78], edx
  50f079: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f07c: 8b 0d f0 fd ca 00            	mov	ecx, dword ptr [0xcafdf0]
  50f082: 89 48 08                     	mov	dword ptr [eax + 0x8], ecx
  50f085: 8b 55 e4                     	mov	edx, dword ptr [ebp - 0x1c]
  50f088: 8b 02                        	mov	eax, dword ptr [edx]
  50f08a: 8b 48 28                     	mov	ecx, dword ptr [eax + 0x28]
  50f08d: 89 4d d4                     	mov	dword ptr [ebp - 0x2c], ecx
  50f090: 83 7d d4 03                  	cmp	dword ptr [ebp - 0x2c], 0x3
  50f094: 0f 87 7a 01 00 00            	ja	0x50f214 <PHYSFSRWOPS_openAppend+0x10b784>
  50f09a: 8b 55 d4                     	mov	edx, dword ptr [ebp - 0x2c]
  50f09d: ff 24 95 c0 f8 50 00         	jmp	dword ptr [4*edx + 0x50f8c0]
  50f0a4: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f0a7: c7 40 0c 00 00 00 00         	mov	dword ptr [eax + 0xc], 0x0
  50f0ae: e9 61 01 00 00               	jmp	0x50f214 <PHYSFSRWOPS_openAppend+0x10b784>
  50f0b3: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50f0b6: 8b 11                        	mov	edx, dword ptr [ecx]
  50f0b8: 83 7a 30 00                  	cmp	dword ptr [edx + 0x30], 0x0
  50f0bc: 74 37                        	je	0x50f0f5 <PHYSFSRWOPS_openAppend+0x10b665>
  50f0be: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f0c1: 8b 30                        	mov	esi, dword ptr [eax]
  50f0c3: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50f0c6: 8b 11                        	mov	edx, dword ptr [ecx]
  50f0c8: db 42 30                     	fild	dword ptr [edx + 0x30]
  50f0cb: 51                           	push	ecx
  50f0cc: d9 1c 24                     	fstp	dword ptr [esp]
  50f0cf: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f0d2: 8b 08                        	mov	ecx, dword ptr [eax]
  50f0d4: 8b 51 30                     	mov	edx, dword ptr [ecx + 0x30]
  50f0d7: f7 da                        	neg	edx
  50f0d9: 89 55 d0                     	mov	dword ptr [ebp - 0x30], edx
  50f0dc: db 45 d0                     	fild	dword ptr [ebp - 0x30]
  50f0df: 51                           	push	ecx
  50f0e0: d9 1c 24                     	fstp	dword ptr [esp]
  50f0e3: b9 00 1a d3 00               	mov	ecx, 0xd31a00
  50f0e8: e8 13 7e ef ff               	call	0x406f00 <PHYSFSRWOPS_openAppend+0x3470>
  50f0ed: 03 46 2c                     	add	eax, dword ptr [esi + 0x2c]
  50f0f0: 89 45 cc                     	mov	dword ptr [ebp - 0x34], eax
  50f0f3: eb 0b                        	jmp	0x50f100 <PHYSFSRWOPS_openAppend+0x10b670>
  50f0f5: 8b 45 e4                     	mov	eax, dword ptr [ebp - 0x1c]
  50f0f8: 8b 08                        	mov	ecx, dword ptr [eax]
  50f0fa: 8b 51 2c                     	mov	edx, dword ptr [ecx + 0x2c]
  50f0fd: 89 55 cc                     	mov	dword ptr [ebp - 0x34], edx
  50f100: a1 f0 fd ca 00               	mov	eax, dword ptr [0xcafdf0]
  50f105: 03 45 cc                     	add	eax, dword ptr [ebp - 0x34]
  50f108: 8b 4d e4                     	mov	ecx, dword ptr [ebp - 0x1c]
  50f10b: 89 41 0c                     	mov	dword ptr [ecx + 0xc], eax
  50f10e: e9 01 01 00 00               	jmp	0x50f214 <PHYSFSRWOPS_openAppend+0x10b784>

; manager lookup uses coordinate comparator

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  510ce0: 55                           	push	ebp
  510ce1: 8b ec                        	mov	ebp, esp
  510ce3: 83 ec 08                     	sub	esp, 0x8
  510ce6: 89 4d f8                     	mov	dword ptr [ebp - 0x8], ecx
  510ce9: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  510cec: 83 c1 14                     	add	ecx, 0x14
  510cef: e8 ec 86 4a 00               	call	0x9b93e0 <PHYSFSRWOPS_openAppend+0x5b5950>
  510cf4: 0f b6 c0                     	movzx	eax, al
  510cf7: 85 c0                        	test	eax, eax
  510cf9: 75 4d                        	jne	0x510d48 <PHYSFSRWOPS_openAppend+0x10d2b8>
  510cfb: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  510cfe: 83 c1 14                     	add	ecx, 0x14
  510d01: e8 1a ac 4a 00               	call	0x9bb920 <PHYSFSRWOPS_openAppend+0x5b7e90>
  510d06: 83 e8 01                     	sub	eax, 0x1
  510d09: 89 45 fc                     	mov	dword ptr [ebp - 0x4], eax
  510d0c: eb 09                        	jmp	0x510d17 <PHYSFSRWOPS_openAppend+0x10d287>
  510d0e: 8b 4d fc                     	mov	ecx, dword ptr [ebp - 0x4]
  510d11: 83 e9 01                     	sub	ecx, 0x1
  510d14: 89 4d fc                     	mov	dword ptr [ebp - 0x4], ecx
  510d17: 83 7d fc 00                  	cmp	dword ptr [ebp - 0x4], 0x0
  510d1b: 7c 2b                        	jl	0x510d48 <PHYSFSRWOPS_openAppend+0x10d2b8>
  510d1d: 8b 55 0c                     	mov	edx, dword ptr [ebp + 0xc]
  510d20: 52                           	push	edx
  510d21: 8b 45 08                     	mov	eax, dword ptr [ebp + 0x8]
  510d24: 50                           	push	eax
  510d25: 8b 4d fc                     	mov	ecx, dword ptr [ebp - 0x4]
  510d28: 51                           	push	ecx
  510d29: 8b 4d f8                     	mov	ecx, dword ptr [ebp - 0x8]
  510d2c: 83 c1 14                     	add	ecx, 0x14
  510d2f: e8 fc 7c 4a 00               	call	0x9b8a30 <PHYSFSRWOPS_openAppend+0x5b4fa0>
  510d34: 8b 08                        	mov	ecx, dword ptr [eax]
  510d36: e8 55 40 f4 ff               	call	0x454d90 <PHYSFSRWOPS_openAppend+0x51300>
  510d3b: 0f b6 d0                     	movzx	edx, al
  510d3e: 85 d2                        	test	edx, edx
  510d40: 74 04                        	je	0x510d46 <PHYSFSRWOPS_openAppend+0x10d2b6>
  510d42: b0 01                        	mov	al, 0x1
  510d44: eb 04                        	jmp	0x510d4a <PHYSFSRWOPS_openAppend+0x10d2ba>
  510d46: eb c6                        	jmp	0x510d0e <PHYSFSRWOPS_openAppend+0x10d27e>
  510d48: 32 c0                        	xor	al, al
  510d4a: 8b e5                        	mov	esp, ebp
  510d4c: 5d                           	pop	ebp

; coordinate comparator implementation

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  454d90: 55                           	push	ebp
  454d91: 8b ec                        	mov	ebp, esp
  454d93: 83 ec 08                     	sub	esp, 0x8
  454d96: 89 4d fc                     	mov	dword ptr [ebp - 0x4], ecx
  454d99: 8b 45 fc                     	mov	eax, dword ptr [ebp - 0x4]
  454d9c: 8b 08                        	mov	ecx, dword ptr [eax]
  454d9e: 3b 4d 08                     	cmp	ecx, dword ptr [ebp + 0x8]
  454da1: 75 1f                        	jne	0x454dc2 <PHYSFSRWOPS_openAppend+0x51332>
  454da3: 8b 55 0c                     	mov	edx, dword ptr [ebp + 0xc]
  454da6: 52                           	push	edx
  454da7: 8b 4d fc                     	mov	ecx, dword ptr [ebp - 0x4]
  454daa: 83 c1 10                     	add	ecx, 0x10
  454dad: e8 ee 4f fb ff               	call	0x409da0 <PHYSFSRWOPS_openAppend+0x6310>
  454db2: 0f b6 c0                     	movzx	eax, al
  454db5: 85 c0                        	test	eax, eax
  454db7: 74 09                        	je	0x454dc2 <PHYSFSRWOPS_openAppend+0x51332>
  454db9: c7 45 f8 01 00 00 00         	mov	dword ptr [ebp - 0x8], 0x1
  454dc0: eb 07                        	jmp	0x454dc9 <PHYSFSRWOPS_openAppend+0x51339>
  454dc2: c7 45 f8 00 00 00 00         	mov	dword ptr [ebp - 0x8], 0x0
  454dc9: 8a 45 f8                     	mov	al, byte ptr [ebp - 0x8]
  454dcc: 8b e5                        	mov	esp, ebp
  454dce: 5d                           	pop	ebp
  454dcf: c2 08 00                     	ret	0x8

; vector erase decrements/erases through container helper

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  9e4150: 55                           	push	ebp
  9e4151: 8b ec                        	mov	ebp, esp
  9e4153: 83 ec 10                     	sub	esp, 0x10
  9e4156: 8b 45 0c                     	mov	eax, dword ptr [ebp + 0xc]
  9e4159: 50                           	push	eax
  9e415a: 8d 4d f4                     	lea	ecx, [ebp - 0xc]
  9e415d: 51                           	push	ecx
  9e415e: 8d 55 f8                     	lea	edx, [ebp - 0x8]
  9e4161: 52                           	push	edx
  9e4162: 8b 4d 08                     	mov	ecx, dword ptr [ebp + 0x8]
  9e4165: e8 a6 da fd ff               	call	0x9c1c10 <PHYSFSRWOPS_openAppend+0x5be180>
  9e416a: 8b c8                        	mov	ecx, eax
  9e416c: e8 4f 74 00 00               	call	0x9eb5c0 <PHYSFSRWOPS_openAppend+0x5e7b30>
  9e4171: 8b 00                        	mov	eax, dword ptr [eax]
  9e4173: 89 45 fc                     	mov	dword ptr [ebp - 0x4], eax
  9e4176: 8b 4d fc                     	mov	ecx, dword ptr [ebp - 0x4]
  9e4179: 51                           	push	ecx
  9e417a: 8d 55 f0                     	lea	edx, [ebp - 0x10]
  9e417d: 52                           	push	edx
  9e417e: 8b 4d 08                     	mov	ecx, dword ptr [ebp + 0x8]
  9e4181: e8 1a 66 00 00               	call	0x9ea7a0 <PHYSFSRWOPS_openAppend+0x5e6d10>
  9e4186: 8b e5                        	mov	esp, ebp
  9e4188: 5d                           	pop	ebp
  9e4189: c3                           	ret

; action cleanup/release wrapper

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  48c230: 55                           	push	ebp
  48c231: 8b ec                        	mov	ebp, esp
  48c233: 83 ec 0c                     	sub	esp, 0xc
  48c236: 89 4d f8                     	mov	dword ptr [ebp - 0x8], ecx
  48c239: 8b 45 f8                     	mov	eax, dword ptr [ebp - 0x8]
  48c23c: 83 c0 04                     	add	eax, 0x4
  48c23f: 83 ec 1c                     	sub	esp, 0x1c
  48c242: 8b cc                        	mov	ecx, esp
  48c244: 89 65 fc                     	mov	dword ptr [ebp - 0x4], esp
  48c247: 50                           	push	eax
  48c248: e8 43 45 52 00               	call	0x9b0790 <PHYSFSRWOPS_openAppend+0x5acd00>
  48c24d: 89 45 f4                     	mov	dword ptr [ebp - 0xc], eax
  48c250: 8b 4d 08                     	mov	ecx, dword ptr [ebp + 0x8]
  48c253: 51                           	push	ecx
  48c254: e8 d7 d5 f7 ff               	call	0x409830 <PHYSFSRWOPS_openAppend+0x5da0>
  48c259: 83 c4 20                     	add	esp, 0x20
  48c25c: 8b 55 f8                     	mov	edx, dword ptr [ebp - 0x8]
  48c25f: 83 c2 20                     	add	edx, 0x20
  48c262: 52                           	push	edx
  48c263: 8b 45 08                     	mov	eax, dword ptr [ebp + 0x8]
  48c266: 50                           	push	eax
  48c267: e8 94 80 54 00               	call	0x9d4300 <PHYSFSRWOPS_openAppend+0x5d0870>
  48c26c: 83 c4 08                     	add	esp, 0x8
  48c26f: 8b e5                        	mov	esp, ebp
  48c271: 5d                           	pop	ebp
  48c272: c2 04 00                     	ret	0x4

; action record destructor

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  454cf0: 55                           	push	ebp
  454cf1: 8b ec                        	mov	ebp, esp
  454cf3: 6a ff                        	push	-0x1
  454cf5: 68 67 f1 aa 00               	push	0xaaf167
  454cfa: 64 a1 00 00 00 00            	mov	eax, dword ptr fs:[0x0]
  454d00: 50                           	push	eax
  454d01: 51                           	push	ecx
  454d02: a1 44 b1 ca 00               	mov	eax, dword ptr [0xcab144]
  454d07: 33 c5                        	xor	eax, ebp
  454d09: 50                           	push	eax
  454d0a: 8d 45 f4                     	lea	eax, [ebp - 0xc]
  454d0d: 64 a3 00 00 00 00            	mov	dword ptr fs:[0x0], eax
  454d13: 89 4d f0                     	mov	dword ptr [ebp - 0x10], ecx
  454d16: c7 45 fc 02 00 00 00         	mov	dword ptr [ebp - 0x4], 0x2
  454d1d: 8b 4d f0                     	mov	ecx, dword ptr [ebp - 0x10]
  454d20: 81 c1 a4 00 00 00            	add	ecx, 0xa4
  454d26: e8 35 13 56 00               	call	0x9b6060 <PHYSFSRWOPS_openAppend+0x5b25d0>
  454d2b: c6 45 fc 01                  	mov	byte ptr [ebp - 0x4], 0x1
  454d2f: 8b 4d f0                     	mov	ecx, dword ptr [ebp - 0x10]
  454d32: 81 c1 94 00 00 00            	add	ecx, 0x94
  454d38: e8 23 13 56 00               	call	0x9b6060 <PHYSFSRWOPS_openAppend+0x5b25d0>
  454d3d: c6 45 fc 00                  	mov	byte ptr [ebp - 0x4], 0x0
  454d41: 8b 4d f0                     	mov	ecx, dword ptr [ebp - 0x10]
  454d44: 81 c1 84 00 00 00            	add	ecx, 0x84
  454d4a: e8 a1 fa 55 00               	call	0x9b47f0 <PHYSFSRWOPS_openAppend+0x5b0d60>
  454d4f: c7 45 fc ff ff ff ff         	mov	dword ptr [ebp - 0x4], 0xffffffff
  454d56: 8b 4d f0                     	mov	ecx, dword ptr [ebp - 0x10]
  454d59: 83 c1 38                     	add	ecx, 0x38
  454d5c: e8 1f b8 fb ff               	call	0x410580 <PHYSFSRWOPS_openAppend+0xcaf0>
  454d61: 8b 4d f4                     	mov	ecx, dword ptr [ebp - 0xc]
  454d64: 64 89 0d 00 00 00 00         	mov	dword ptr fs:[0x0], ecx
  454d6b: 59                           	pop	ecx
  454d6c: 8b e5                        	mov	esp, ebp
  454d6e: 5d                           	pop	ebp
  454d6f: c3                           	ret

; map effect callsite: helper output and source are passed to manager/action

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  7b148a: 6a 09                        	push	0x9
  7b148c: 6a 00                        	push	0x0
  7b148e: 6a 00                        	push	0x0
  7b1490: 68 80 cf cf 00               	push	0xcfcf80
  7b1495: 8b 4d 0c                     	mov	ecx, dword ptr [ebp + 0xc]
  7b1498: 51                           	push	ecx
  7b1499: 8d 95 88 fb ff ff            	lea	edx, [ebp - 0x478]
  7b149f: 52                           	push	edx
  7b14a0: 8b 8d 84 fb ff ff            	mov	ecx, dword ptr [ebp - 0x47c]
  7b14a6: e8 85 01 00 00               	call	0x7b1630 <PHYSFSRWOPS_openAppend+0x3adba0>
  7b14ab: 50                           	push	eax
  7b14ac: 8b 45 08                     	mov	eax, dword ptr [ebp + 0x8]
  7b14af: 50                           	push	eax
  7b14b0: 8b 8d 84 fb ff ff            	mov	ecx, dword ptr [ebp - 0x47c]
  7b14b6: 8b 51 64                     	mov	edx, dword ptr [ecx + 0x64]
  7b14b9: 52                           	push	edx
  7b14ba: 8b 85 84 fb ff ff            	mov	eax, dword ptr [ebp - 0x47c]
  7b14c0: 8b 48 64                     	mov	ecx, dword ptr [eax + 0x64]
  7b14c3: e8 78 f7 d5 ff               	call	0x510c40 <PHYSFSRWOPS_openAppend+0x10d1b0>
  7b14c8: 8b c8                        	mov	ecx, eax
  7b14ca: e8 31 da d5 ff               	call	0x50ef00 <PHYSFSRWOPS_openAppend+0x10b470>
  7b14cf: 33 c9                        	xor	ecx, ecx
  7b14d1: 75 b7                        	jne	0x7b148a <PHYSFSRWOPS_openAppend+0x3ad9fa>
  7b14d3: 5e                           	pop	esi
  7b14d4: 8b e5                        	mov	esp, ebp
  7b14d6: 5d                           	pop	ebp
  7b14d7: c2 14 00                     	ret	0x14

; helper returns its output-object pointer and cleans two stack arguments

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  7b196e: 6a 00                        	push	0x0
  7b1970: 6a 00                        	push	0x0
  7b1972: 8b 4d 08                     	mov	ecx, dword ptr [ebp + 0x8]
  7b1975: e8 f6 00 cb ff               	call	0x461a70 <PHYSFSRWOPS_openAppend+0x5dfe0>
  7b197a: 8b 45 08                     	mov	eax, dword ptr [ebp + 0x8]
  7b197d: 5e                           	pop	esi
  7b197e: 8b e5                        	mov	esp, ebp
  7b1980: 5d                           	pop	ebp
  7b1981: c2 08 00                     	ret	0x8

; pair copy helper copies two dwords from the supplied source pointer

original/bazzite/COGMIND.exe:	file format coff-i386

Disassembly of section .text:

00403a90 <PHYSFSRWOPS_openAppend>:
  40c060: 55                           	push	ebp
  40c061: 8b ec                        	mov	ebp, esp
  40c063: 51                           	push	ecx
  40c064: 89 4d fc                     	mov	dword ptr [ebp - 0x4], ecx
  40c067: 8b 45 fc                     	mov	eax, dword ptr [ebp - 0x4]
  40c06a: 8b 4d 08                     	mov	ecx, dword ptr [ebp + 0x8]
  40c06d: 8b 11                        	mov	edx, dword ptr [ecx]
  40c06f: 89 10                        	mov	dword ptr [eax], edx
  40c071: 8b 45 fc                     	mov	eax, dword ptr [ebp - 0x4]
  40c074: 8b 4d 08                     	mov	ecx, dword ptr [ebp + 0x8]
  40c077: 8b 51 04                     	mov	edx, dword ptr [ecx + 0x4]
  40c07a: 89 50 04                     	mov	dword ptr [eax + 0x4], edx
  40c07d: 8b 45 fc                     	mov	eax, dword ptr [ebp - 0x4]
  40c080: 8b e5                        	mov	esp, ebp
  40c082: 5d                           	pop	ebp
  40c083: c2 04 00                     	ret	0x4
