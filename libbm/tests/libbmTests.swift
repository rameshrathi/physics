//
//  libbmTests.swift
//  libbm
//

import Testing
import libbm

struct LibbmTest {
	
	@Test("list test")
	func testList() {
		var items : [Int32] = [1, 2, 3, 4, 5, 6, 7, 8, 9]
		let root =  create_list(&items, 9)
		free_list(root)
	}
	
}
