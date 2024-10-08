#include "pdffile.h"

expected<void, std::string> PdfFile::open(const std::string &file_path) {
    guard("PdfFile::open");
    __document.reset(poppler::document::load_from_file(file_path));
    if (!__document) {
        return unexpected<std::string>("Error: Unable to open PDF document: " + file_path);
    }
    unguard();
    return {};
}

void PdfFile::close() {
    __document.reset();
}

expected<std::string, std::string> PdfFile::read() {
    guard("PdfFile::read");
    if (!__document) {
        return unexpected<std::string>("Error: No document is open.");
    }
    std::stringstream content;
    int num_pages = __document->pages();
    for (int i = 0; i < num_pages; ++i) {
        std::unique_ptr<poppler::page> page(__document->create_page(i));
        if (!page) {
            return unexpected<std::string>("Error: Unable to load page " + std::to_string(i));
        }
        content << "Page " << i + 1 << ":\n";
        content << page->text().to_latin1() << "\n";
    }
    return content.str();
    unguard();
    return {};
}

expected<std::string, std::string> PdfFile::read_file(const std::string& file_path) {
    auto file_res = open(file_path);
    if (!file_res) {
        unexpected(file_res.error());
    }
    auto read_res = read();
    if (!read_res) {
        unexpected(read_res.error());
    }
    close();
    return read_res.value();
}
