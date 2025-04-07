import Testing
import Foundation

@testable import LibLlma

// Assuming LlamaKit.swift (containing the wrapper above) is part of the test target

@Suite("LlamaSession Tests")
struct LlamaSessionTests {

	// --- Test Data ---
	// IMPORTANT: Replace with a *real*, small GGUF model path for integration testing.
	// For unit tests without a model, we might use dummy paths and mock file existence.
	let validModelPath = "/Users/ramesh/Documents/docs/mmproj-model-f16-12B.gguf" // A path the mock loader treats as valid
	let invalidModelPath = "nonexistent/model.gguf"
	let nonGGUFPath = "path/to/some/file.txt"
	let tempDirectory = FileManager.default.temporaryDirectory.appendingPathComponent("LlamaTests")

	// --- Setup & Teardown ---
	// Create a dummy valid file for testing load success (optional, depends on mock)
	init() {
		try? FileManager.default.createDirectory(at: tempDirectory, withIntermediateDirectories: true)
		// Create a dummy file IF your mock requires actual file existence check
		// let dummyModelURL = tempDirectory.appendingPathComponent("dummy.gguf")
		// if !FileManager.default.fileExists(atPath: dummyModelURL.path) {
		//     FileManager.default.createFile(atPath: dummyModelURL.path, contents: Data("dummy".utf8))
		//     validModelPath = dummyModelURL.path // Update path if dummy file is created
		// }
		print("Test Suite Initialized. Using valid path: \(validModelPath)")
	}

//	// Clean up dummy files/directories after tests if created
//	@Test(arguments: [
//		\LlamaSessionTests.validModelPath,
//		 \LlamaSessionTests.invalidModelPath,
//		 \LlamaSessionTests.nonGGUFPath
//	]) // Example of passing args
//	static func teardown(_ vmp: String, _ imp: String, _ ngp: String) {
//		// Access shared state or perform cleanup if needed
//		// In this simple case, deinit of LlamaSession handles resource freeing in the mock.
//		 let tempDirectory = FileManager.default.temporaryDirectory.appendingPathComponent("LlamaTests")
//		 try? FileManager.default.removeItem(at: tempDirectory)
//		 print("Test Suite Teardown Complete.")
//	}

	// --- Test Cases ---
	@Test("Initialize LlamaSession Successfully")
	func testInitializationSuccess() async throws {
		// #require will fail the test immediately if the condition isn't met
		let session = try LlamaSession(modelPath: validModelPath)
		#expect(session.isInitialized == true)
		#expect(session.getContextSize() > 0) // Check basic parameter access
	}

	@Test("Initialize LlamaSession Failure - File Not Found")
	func testInitializationFileNotFound() async throws {
		#expect(throws: LlamaError.self) { // Expect *any* LlamaError
			_ = try LlamaSession(modelPath: invalidModelPath)
		}

		// More specific check (if your error conforms to Equatable)
		#expect(throws: LlamaError.fileNotFound(path: invalidModelPath)) {
			 _ = try LlamaSession(modelPath: invalidModelPath)
		}
	}

	@Test("Initialize LlamaSession Failure - Invalid File Type")
	 func testInitializationInvalidFileType() async throws {
		 // Need a dummy file that exists but has the wrong extension for the mock
		 let dummyTextFileURL = tempDirectory.appendingPathComponent("dummy.txt")
		 FileManager.default.createFile(atPath: dummyTextFileURL.path, contents: Data("test".utf8))

		 #expect(throws: LlamaError.modelLoadFailed(reason: "Invalid file type")) {
			 _ = try LlamaSession(modelPath: dummyTextFileURL.path)
		 }
		 // Clean up the dummy file
		 try? FileManager.default.removeItem(at: dummyTextFileURL)
	 }


	@Test("Tokenization - Basic")
	func testTokenizationBasic() async throws {
		let session = try LlamaSession(modelPath: validModelPath)
		let text = "Hello world"
		let tokens = try session.tokenize(text: text)

		#expect(!tokens.isEmpty)
		// In the mock, tokens are ASCII + 1000
		#expect(tokens.count == text.count) // Simple check based on mock logic
		#expect(tokens.first == Int32(Character("H").asciiValue!) + 1000)
	}

	@Test("Tokenization - Empty String")
	func testTokenizationEmpty() async throws {
		let session = try LlamaSession(modelPath: validModelPath)
		let tokens = try session.tokenize(text: "")
		#expect(tokens.isEmpty)
	}

	@Test("Generation - Basic Prompt")
	func testGenerationBasic() async throws {
		let session = try LlamaSession(modelPath: validModelPath)
		let prompt = "This is a test prompt."
		// Use default parameters for generation in this test
		let result = try session.generate(prompt: prompt)

		#expect(!result.isEmpty)
		// Cannot reliably test *content* without a real model & complex setup.
		// Test that *something* was generated. Our mock adds '<EOS>' or 'word_X'
		#expect(result.contains("word_") || result.contains("<EOS>"))
		print("Generated Output (Mock): \(result)")
	}

	 @Test("Generation - With Max Tokens Limit")
	 func testGenerationMaxTokens() async throws {
		 let params = LlamaParameters()
		 // Note: Our *mock* `generate` doesn't fully respect this param yet,
		 // but we test the API call structure. A real test would need a real model.
		 let session = try LlamaSession(modelPath: validModelPath, parameters: params)
		 let prompt = "Generate a short story."
		 let maxTokens = 5 // Ask for very few tokens

		 let result = try session.generate(prompt: prompt, maxTokens: maxTokens)

		 #expect(!result.isEmpty)
		 // We can't easily count tokens in the output without the real tokenizer logic,
		 // but we expect the mock to have run the loop at most `maxTokens` times.
		 // (Verification relies on checking the mock's print statements or refining the mock)
		 print("Generated Output (Mock, maxTokens=5): \(result)")
	 }

	@Test("Resource Deinitialization")
	func testResourceDeinitialization() async throws {
		// This test relies on ARC calling deinit.
		var session: LlamaSession? = try? LlamaSession(modelPath: validModelPath)
		#expect(session != nil)
		#expect(session!.isInitialized == true)
		// Let ARC release the session
		session = nil
	}

	// Add more tests for:
	// - Different parameter settings (temperature, topK, topP, etc.) -> Check mock prints params
	// - Handling very long prompts (might exceed context) -> Need mock logic for this
	// - Error conditions during evaluation/sampling (mock needs to simulate these)
	// - GPU offloading parameter -> Check mock prints params
}

// --- Helper Extensions ---
// Make the custom error equatable for easier testing
extension LlamaError {
	static func == (lhs: LlamaError, rhs: LlamaError) -> Bool {
		switch (lhs, rhs) {
		case (.modelLoadFailed(let r1), .modelLoadFailed(let r2)):
			return r1 == r2
		case (.contextCreationFailed(let r1), .contextCreationFailed(let r2)):
			return r1 == r2
		case (.tokenizationFailed, .tokenizationFailed):
			return true
		case (.evaluationFailed(let r1), .evaluationFailed(let r2)):
			return r1 == r2
		case (.samplingFailed(let r1), .samplingFailed(let r2)):
			return r1 == r2
		case (.invalidParameters(let p1), .invalidParameters(let p2)):
			return p1 == p2
		case (.sessionNotInitialized, .sessionNotInitialized):
			return true
		 case (.fileNotFound(let p1), .fileNotFound(let p2)):
			return p1 == p2
		default:
			return false
		}
	}
}

// Allow LlamaSession to be used where non-nil is required easily
extension LlamaSession: Equatable {
	public static func == (lhs: LlamaSession, rhs: LlamaSession) -> Bool {
		// Basic equality check based on object identity for testing purposes
		return lhs === rhs
	}
}
