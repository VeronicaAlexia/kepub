#include "epub/epub.h"

#include <filesystem>
#include <memory>
#include <string_view>

#include <fmt/ostream.h>
#include <unicode/translit.h>
#include <unicode/unistr.h>
#include <unicode/utypes.h>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "epub/font.h"
#include "epub/version.h"

namespace {

inline std::unique_ptr<icu::Transliterator> trans;

std::string num_to_str(std::int32_t i) {
  if (i <= 0 || i >= 1000) {
    error("too many xhtml files need to be generated");
  }

  if (i < 10) {
    return "00" + std::to_string(i);
  } else if (i < 100) {
    return "0" + std::to_string(i);
  } else {
    return std::to_string(i);
  }
}

void create_directory(const std::string &dir) {
  if (!std::filesystem::create_directory(dir)) {
    error("can not create directory: {}", dir);
  }
}

void custom_trans(icu::UnicodeString &str) {
  str.findAndReplace("&", "&amp;");
  str.findAndReplace("<", "&lt;");
  str.findAndReplace(">", "&gt;");

  str.findAndReplace("妳", "你");
  str.findAndReplace("壊", "坏");
  str.findAndReplace("拚", "拼");
  str.findAndReplace("噁", "恶");
  str.findAndReplace("歳", "岁");
  str.findAndReplace("経", "经");
  str.findAndReplace("験", "验");
  str.findAndReplace("険", "险");
  str.findAndReplace("撃", "击");
  str.findAndReplace("錬", "炼");
  str.findAndReplace("隷", "隶");
  str.findAndReplace("毎", "每");
  str.findAndReplace("捩", "折");
  str.findAndReplace("殻", "壳");
  str.findAndReplace("牠", "它");
  str.findAndReplace("矇", "蒙");
  str.findAndReplace("髮", "发");
  str.findAndReplace("姊", "姐");
  str.findAndReplace("黒", "黑");
  str.findAndReplace("歴", "历");
  str.findAndReplace("様", "样");
  str.findAndReplace("甦", "苏");
  str.findAndReplace("牴", "抵");
  str.findAndReplace("銀", "银");
  str.findAndReplace("齢", "龄");
  str.findAndReplace("祂", "他");
  str.findAndReplace("従", "从");
  str.findAndReplace("酔", "醉");
  str.findAndReplace("値", "值");
  str.findAndReplace("発", "发");
  str.findAndReplace("続", "续");
  str.findAndReplace("転", "转");
  str.findAndReplace("剣", "剑");
  str.findAndReplace("砕", "碎");
  str.findAndReplace("鉄", "铁");
  str.findAndReplace("甯", "宁");
  str.findAndReplace("鬪", "斗");
  str.findAndReplace("寛", "宽");
  str.findAndReplace("変", "变");
  str.findAndReplace("鳮", "鸡");
  str.findAndReplace("悪", "恶");
  str.findAndReplace("霊", "灵");
  str.findAndReplace("戦", "战");
  str.findAndReplace("権", "权");
  str.findAndReplace("効", "效");
  str.findAndReplace("応", "应");
  str.findAndReplace("覚", "觉");
  str.findAndReplace("観", "观");
  str.findAndReplace("気", "气");
  str.findAndReplace("歴", "历");
  str.findAndReplace("覧", "览");
  str.findAndReplace("殭", "僵");
  str.findAndReplace("郞", "郎");
  str.findAndReplace("虊", "药");
  str.findAndReplace("踼", "踢");
  str.findAndReplace("逹", "达");
  str.findAndReplace("鑜", "锁");
  str.findAndReplace("髲", "发");
  str.findAndReplace("髪", "发");
  str.findAndReplace("実", "实");
  str.findAndReplace("內", "内");
  str.findAndReplace("穨", "颓");
  str.findAndReplace("糸", "系");
  str.findAndReplace("賍", "赃");
  str.findAndReplace("掦", "扬");
  str.findAndReplace("覇", "霸");
  str.findAndReplace("姉", "姐");
  str.findAndReplace("楽", "乐");
  str.findAndReplace("継", "继");
  str.findAndReplace("隠", "隐");
  str.findAndReplace("巻", "卷");
  str.findAndReplace("膞", "膊");
  str.findAndReplace("髑", "骷");
  str.findAndReplace("劄", "札");
  str.findAndReplace("擡", "抬");
  str.findAndReplace("⼈", "人");
  str.findAndReplace("⾛", "走");
  str.findAndReplace("⼤", "大");
  str.findAndReplace("⽤", "用");
  str.findAndReplace("⼿", "手");
  str.findAndReplace("⼦", "子");
  str.findAndReplace("⽽", "而");
  str.findAndReplace("⾄", "至");
  str.findAndReplace("⽯", "石");
  str.findAndReplace("⼗", "十");
  str.findAndReplace("⽩", "白");
  str.findAndReplace("⽗", "父");
  str.findAndReplace("⽰", "示");
  str.findAndReplace("⾁", "肉");
  str.findAndReplace("⼠", "士");
  str.findAndReplace("⽌", "止");
  str.findAndReplace("⼀", "一");
  str.findAndReplace("⺠", "民");
}

}  // namespace

void init_trans() {
  if (!trans) {
    UErrorCode status{U_ZERO_ERROR};
    trans = std::unique_ptr<icu::Transliterator>(
        icu::Transliterator::createInstance("Hant-Hans", UTRANS_FORWARD,
                                            status));

    if (U_FAILURE(status)) {
      error("error: {}", u_errorName(status));
    }
  }
}

std::string trans_str(const std::string &str) {
  icu::UnicodeString icu_str{str.c_str()};
  icu_str.trim();

  trans->transliterate(icu_str);
  custom_trans(icu_str);

  std::string temp;
  return icu_str.toUTF8String(temp);
}

std::string get_chapter_filename(const std::string &book_name,
                                 std::int32_t count) {
  return book_name + "/OEBPS/Text/" + "chapter" + num_to_str(count) + ".xhtml";
}

void check_file_is_open(const std::ifstream &file,
                        const std::string &filename) {
  if (!file) {
    error("can not open file: {}", filename);
  }
}

void check_file_is_open(const std::ofstream &file,
                        const std::string &filename) {
  if (!file) {
    error("can not create file: {}", filename);
  }
}

std::string chapter_file_begin(const std::string &title) {
  return "<?xml version='1.0' encoding='utf-8'?>\n"
         "<html\n"
         "  xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "  xmlns:epub=\"http://www.idpf.org/2007/ops\"\n"
         "  xml:lang=\"zh-CN\"\n"
         ">\n"
         "  <head>\n"
         "    <title>" +
         title +
         "</title>\n"
         "    <meta http-equiv=\"Content-Type\" content=\"text/html; "
         "charset=utf-8\" />\n"
         "    <link href=\"../../stylesheet.css\" rel=\"stylesheet\" "
         "type=\"text/css\" />\n"
         "    <link href=\"../../page_styles.css\" rel=\"stylesheet\" "
         "type=\"text/css\" />\n"
         "  </head>\n"
         "  <body class=\"calibre\">\n"
         "    <div class=\"calibre1\">\n"
         "      <h1 class=\"color\">" +
         title + "</h1>\n";
}

std::string chapter_file_text(const std::string &text) {
  return "      <p class=\"calibre2\">" + text + "</p>\n";
}

const char *chapter_file_end() {
  return "    </div>\n"
         "  </body>\n"
         "</html>\n";
}

std::pair<std::vector<std::string>, bool> processing_cmd(std::int32_t argc,
                                                         char *argv[]) {
  boost::program_options::options_description generic{"Generic options"};
  generic.add_options()("version,v", "print version string");
  generic.add_options()("help,h", "produce help message");

  boost::program_options::options_description config{"Configuration"};
  config.add_options()("xhtml,x", "only generate xhtml file");

  boost::program_options::options_description hidden{"Hidden options"};
  hidden.add_options()(
      "input-file", boost::program_options::value<std::vector<std::string>>(),
      "input file");

  boost::program_options::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  boost::program_options::options_description visible{"Allowed options"};
  visible.add(generic).add(config);

  boost::program_options::positional_options_description p;
  p.add("input-file", -1);

  boost::program_options::variables_map vm;
  store(boost::program_options::command_line_parser(argc, argv)
            .options(cmdline_options)
            .positional(p)
            .run(),
        vm);
  notify(vm);

  if (vm.contains("help")) {
    fmt::print("Usage: {} [options] file...\n\n{}\n", argv[0], visible);
    std::exit(EXIT_SUCCESS);
  }

  if (vm.contains("version")) {
    fmt::print("{} version: {}.{}.{}\n", argv[0], EPUB_VER_MAJOR,
               EPUB_VER_MINOR, EPUB_VER_PATCH);
    std::exit(EXIT_SUCCESS);
  }

  if (!vm.contains("input-file")) {
    error("need a text file name");
  }

  return {vm["input-file"].as<std::vector<std::string>>(),
          vm.contains("xhtml")};
}

void create_epub_directory(const std::string &book_name,
                           const std::vector<std::string> &description) {
  if (std::filesystem::exists(book_name) &&
      std::filesystem::is_directory(book_name)) {
    if (std::filesystem::remove_all(book_name) == 0) {
      error("can not remove directory: {}", book_name);
    }
  }

  create_directory(book_name);
  create_directory(book_name + "/META-INF");
  create_directory(book_name + "/OEBPS");
  create_directory(book_name + "/OEBPS/Images");
  create_directory(book_name + "/OEBPS/Fonts");
  create_directory(book_name + "/OEBPS/Text");

  std::ofstream font_ofs{book_name + "/OEBPS/Fonts/MStiffHei PRC Black.ttf"};
  font_ofs << std::string_view(font, font_size) << std::flush;

  std::string filename{book_name + "/stylesheet.css"};
  std::ofstream stylesheet{filename};
  check_file_is_open(stylesheet, filename);

  stylesheet << ".calibre {\n"
                "  display: block;\n"
                "  font-size: 1em;\n"
                "  line-height: 1.2;\n"
                "  text-align: justify;\n"
                "  margin: 0 5pt;\n"
                "  padding: 0% 0;\n"
                "}\n"
                ".calibre1 {\n"
                "  display: block;\n"
                "  line-height: 1.2;\n"
                "  text-align: justify;\n"
                "  margin: 0;\n"
                "  padding: 0;\n"
                "}\n"
                ".calibre2 {\n"
                "  display: block;\n"
                "  line-height: 1.3;\n"
                "  text-indent: 2em;\n"
                "  margin: 0.6em 0;\n"
                "}\n"
                ".calibre3 {\n"
                "  display: block;\n"
                "}\n"
                ".calibre4 {\n"
                "  display: block;\n"
                "  font-size: 1.66667em;\n"
                "  font-weight: bold;\n"
                "  line-height: 1.2;\n"
                "  text-align: center;\n"
                "  margin: 1em 0 1.2em;\n"
                "}\n"
                ".calibre5 {\n"
                "  height: auto;\n"
                "  width: auto;\n"
                "}\n"
                ".calibre6 {\n"
                "  ruby-align: center;\n"
                "}\n"
                ".calibre7 {\n"
                "  font-size: 0.75em;\n"
                "}\n"
                ".center {\n"
                "  display: block;\n"
                "  line-height: 1.2;\n"
                "  text-align: center;\n"
                "  text-indent: 0;\n"
                "  margin: 0;\n"
                "  padding: 0;\n"
                "}\n"
                ".center1 {\n"
                "  display: block;\n"
                "  duokan-bleed: leftright;\n"
                "  line-height: 1.2;\n"
                "  text-align: center;\n"
                "  text-indent: 0;\n"
                "  margin: 0;\n"
                "  padding: 0;\n"
                "}\n"
                ".color {\n"
                "  color: #000;\n"
                "  display: block;\n"
                "  font-size: 1.66667em;\n"
                "  font-weight: bold;\n"
                "  line-height: 1.2;\n"
                "  text-align: center;\n"
                "  margin: 1em 0 1.2em;\n"
                "}\n"
                ".cutline {\n"
                "  display: block;\n"
                "  line-height: 1.2;\n"
                "  text-align: justify;\n"
                "  margin: 0;\n"
                "  padding: 0.25em 0.5em;\n"
                "  border-top: black double medium;\n"
                "  border-bottom: black double medium;\n"
                "}\n"
                ".font {\n"
                "  display: block;\n"
                "  font-size: 0.83333em;\n"
                "  line-height: 1.3;\n"
                "  text-align: center;\n"
                "  text-indent: 0;\n"
                "  margin: 0.6em 0;\n"
                "}\n"
                ".font1 {\n"
                "  color: #000;\n"
                "  display: block;\n"
                "  font-size: 1em;\n"
                "  line-height: 1.3;\n"
                "  text-align: center;\n"
                "  text-indent: 0;\n"
                "  margin: -0.5em 0 0.6em;\n"
                "}\n"
                ".makerifm {\n"
                "  display: block;\n"
                "  line-height: 1.2;\n"
                "  text-indent: 0;\n"
                "  margin: 0.2em 0;\n"
                "}\n"
                ".mt {\n"
                "  display: block;\n"
                "  font-family: black, serif;\n"
                "  font-size: 2em;\n"
                "  line-height: 1.3;\n"
                "  text-align: center;\n"
                "  text-indent: 0;\n"
                "  margin: 1.5em 0 0.6em;\n"
                "}\n"
                ".mt1 {\n"
                "  color: #000;\n"
                "  display: block;\n"
                "  font-family: black, serif;\n"
                "  font-size: 1.66667em;\n"
                "  line-height: 1.3;\n"
                "  text-align: center;\n"
                "  text-indent: 0;\n"
                "  margin: 0.5em 0 0.6em;\n"
                "}\n"
                ".mtb {\n"
                "  display: block;\n"
                "  font-family: black, serif;\n"
                "  font-size: 2em;\n"
                "  line-height: 1.3;\n"
                "  text-align: center;\n"
                "  text-indent: 0;\n"
                "  margin: -0.9em 0 0.6em;\n"
                "}\n"
                ".tco {\n"
                "  color: #fff;\n"
                "  text-shadow: 0 0 5px rgb(147, 75, 95);\n"
                "}\n"
                ".tco1 {\n"
                "  color: #fff;\n"
                "  text-shadow: 0 0 5px rgb(214, 119, 37);\n"
                "}\n"
                ".tco2 {\n"
                "  color: #fff;\n"
                "  text-shadow: 0 0 5px rgb(76, 121, 56);\n"
                "}\n"
                ".tco3 {\n"
                "  color: #fff;\n"
                "  text-shadow: 0 0 5px rgb(17, 82, 102);\n"
                "}\n"
                ".tco4 {\n"
                "  color: #fff;\n"
                "  text-shadow: 0 0 5px rgb(180, 100, 65);\n"
                "}\n"
                ".tco5 {\n"
                "  color: #fff;\n"
                "  text-shadow: 0 0 5px rgb(180, 86, 32);\n"
                "}"
             << std::endl;

  filename = book_name + "/page_styles.css";
  std::ofstream page_styles{filename};
  check_file_is_open(page_styles, filename);

  page_styles << "@page {\n"
                 "  margin-bottom: 5pt;\n"
                 "  margin-top: 5pt;\n"
                 "}\n"
                 "@font-face {\n"
                 "  font-family: \"black\";\n"
                 "  src: url(OEBPS/Fonts/MStiffHei%20PRC%20Black.ttf);\n"
                 "}\n"
                 "@font-face {\n"
                 "  font-family: \"Light\";\n"
                 "  src: url(\"OEBPS/Fonts/MStiffHei PRC Light.ttf.ttf\");\n"
                 "}"
              << std::endl;

  filename = book_name + "/mimetype";
  std::ofstream mimetype{filename};
  check_file_is_open(mimetype, filename);

  mimetype << "application/epub+zip" << std::endl;

  filename = book_name + "/META-INF/container.xml";
  std::ofstream container{filename};
  check_file_is_open(container, filename);

  container << "<?xml version=\"1.0\"?>\n"
               "<container version=\"1.0\" "
               "xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">\n"
               "   <rootfiles>\n"
               "      <rootfile full-path=\"content.opf\" "
               "media-type=\"application/oebps-package+xml\"/>\n"
               "   </rootfiles>\n"
               "</container>"
            << std::endl;

  filename = book_name + "/OEBPS/Text/introduction.xhtml";
  std::ofstream introduction{filename};
  check_file_is_open(introduction, filename);

  introduction << "<?xml version='1.0' encoding='utf-8'?>\n"
                  "<html\n"
                  "  xmlns=\"http://www.w3.org/1999/xhtml\"\n"
                  "  xmlns:epub=\"http://www.idpf.org/2007/ops\"\n"
                  "  xml:lang=\"zh-CN\"\n"
                  ">\n"
                  "  <head>\n"
                  "    <title>简介</title>\n"
                  "    <meta http-equiv=\"Content-Type\" content=\"text/html; "
                  "charset=utf-8\" />\n"
                  "    <link href=\"../../stylesheet.css\" rel=\"stylesheet\" "
                  "type=\"text/css\" />\n"
                  "    <link href=\"../../page_styles.css\" rel=\"stylesheet\" "
                  "type=\"text/css\" />\n"
                  "  </head>\n"
                  "  <body class=\"calibre\">\n"
                  "    <div class=\"calibre1\">\n"
                  "      <h1 class=\"color\">简介</h1>\n";

  for (const auto &item : description) {
    introduction << chapter_file_text(item);
  }

  introduction << "    </div>\n"
                  "  </body>\n"
                  "</html>"
               << std::endl;
}

void generate_xhtml(const std::string &book_name,
                    const std::vector<std::string> &texts) {
  std::string filename{book_name + ".xhtml"};
  std::ofstream xhtml{filename};
  check_file_is_open(xhtml, filename);

  for (const auto &line : texts) {
    xhtml << "<p class=\"calibre2\">" << line << "</p>\n";
  }
}

void generate_content_opf(const std::string &book_name,
                          const std::string &author, std::int32_t count) {
  auto filename{book_name + "/" + "content.opf"};
  std::ofstream ofs{filename};
  check_file_is_open(ofs, filename);

  ofs << "<?xml version=\"1.0\"  encoding=\"UTF-8\"?>\n"
         "<package xmlns=\"http://www.idpf.org/2007/opf\" "
         "unique-identifier=\"uuid_id\" version=\"2.0\">\n"
         "  <metadata "
         "xmlns:calibre=\"http://calibre.kovidgoyal.net/2009/metadata\" "
         "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
         "xmlns:dcterms=\"http://purl.org/dc/terms/\" "
         "xmlns:opf=\"http://www.idpf.org/2007/opf\" "
         "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
         "    <dc:title>"
      << book_name
      << "</dc:title>\n"
         "    <dc:creator opf:role=\"aut\" opf:file-as=\"kaiser\">"
      << author
      << "</dc:creator>\n"
         "    <dc:contributor opf:role=\"bkp\">calibre (4.21.0) "
         "[https://calibre-ebook.com]</dc:contributor>\n"
         "    <dc:rights>kaiser</dc:rights>\n"
         "    <dc:identifier id=\"uuid_id\" "
         "opf:scheme=\"uuid\">b1868e98-d78f-4035-bfe6-ff1e670b6d18</"
         "dc:identifier>\n"
         "    <dc:date>2020-08-02T3:00:00+00:00</dc:date>\n"
         "    <dc:subject>轻小说</dc:subject>\n"
         "    <dc:language>zh</dc:language>\n"
         "    <dc:identifier "
         "opf:scheme=\"calibre\">b1868e98-d78f-4035-bfe6-ff1e670b6d18</"
         "dc:identifier>\n"
         "    <meta name=\"calibre:title_sort\" content=\""
      << book_name
      << "\"/>\n"
         "    <meta name=\"calibre:timestamp\" "
         "content=\"2020-07-31T12:46:51.406000+00:00\"/>\n"
         "    <meta name=\"cover\" content=\"cover\"/>\n"
         "    <meta name=\"calibre:author_link_map\" content=\"{&quot;"
      << author
      << "&quot;: &quot;&quot;}\"/>\n"
         "  </metadata>\n"
         "  <manifest>\n"
         "    <item href=\"OEBPS/Text/introduction.xhtml\" "
         "id=\"introduction.xhtml\" "
         "media-type=\"application/xhtml+xml\"/>\n";

  for (std::int32_t i{1}; i < count; ++i) {
    auto str{num_to_str(i)};
    ofs << "    <item href=\"OEBPS/Text/chapter" << str
        << ".xhtml\" id=\"chapter" << str
        << ".xhtml\" media-type=\"application/xhtml+xml\"/>\n";
  }
  ofs << "    <item href=\"toc.ncx\" id=\"ncx\" "
         "media-type=\"application/x-dtbncx+xml\"/>\n"
         "    <item href=\"page_styles.css\" id=\"page_css\" "
         "media-type=\"text/css\"/>\n"
         "    <item href=\"stylesheet.css\" id=\"css\" "
         "media-type=\"text/css\"/>\n"
         "    <item href=\"OEBPS/Fonts/MStiffHei PRC Black.ttf\" "
         "id=\"MStiffHei_PRC_Black.ttf\" "
         "media-type=\"application/x-font-ttf\"/>\n"
         "</manifest>\n"
         "  <spine toc=\"ncx\">\n"
         "    <itemref idref=\"introduction.xhtml\"/>\n";

  for (std::int32_t i{1}; i < count; ++i) {
    auto str{num_to_str(i)};
    ofs << "    <itemref idref=\"chapter" << str << ".xhtml\"/>\n";
  }

  ofs << "  </spine>\n"
         "  <guide>\n"
         "  </guide>\n"
         "</package>"
      << std::endl;
}

void generate_toc_ncx(const std::string &book_name,
                      const std::vector<std::string> &titles) {
  auto filename{book_name + "/" + "toc.ncx"};
  std::ofstream ofs{filename};
  check_file_is_open(ofs, filename);

  ofs << "<?xml version='1.0' encoding='utf-8'?>\n"
         "<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" "
         "version=\"2005-1\" xml:lang=\"zho\">\n"
         "  <head>\n"
         "    <meta content=\"b1868e98-d78f-4035-bfe6-ff1e670b6d18\" "
         "name=\"dtb:uid\"/>\n"
         "    <meta content=\"2\" name=\"dtb:depth\"/>\n"
         "    <meta content=\"calibre (4.21.0)\" name=\"dtb:generator\"/>\n"
         "    <meta content=\"0\" name=\"dtb:totalPageCount\"/>\n"
         "    <meta content=\"0\" name=\"dtb:maxPageNumber\"/>\n"
         "  </head>\n"
         "  <docTitle>\n"
         "    <text>"
      << book_name
      << "</text>\n"
         "  </docTitle>\n"
         "  <navMap>\n"
         "    <navPoint class=\"chapter\" id=\"navPoint-1\" "
         "playOrder=\"1\">\n"
         "      <navLabel>\n"
         "        <text>简介</text>\n"
         "      </navLabel>\n"
         "      <content src=\"OEBPS/Text/introduction.xhtml\"/>\n"
         "    </navPoint>\n";

  std::int32_t i{2};
  for (const auto &title : titles) {
    ofs << "    <navPoint class=\"chapter\" id=\"navPoint-" << i
        << "\" playOrder=\"" << i
        << "\">\n"
           "      <navLabel>\n"
           "        <text>"
        << title
        << "</text>\n"
           "      </navLabel>\n"
           "      <content src=\"OEBPS/Text/chapter"
        << num_to_str(i - 1)
        << ".xhtml\"/>\n"
           "    </navPoint>\n";
    ++i;
  }

  ofs << "  </navMap>\n"
         "</ncx>"
      << std::endl;
}

std::pair<std::string, std::vector<std::string>> read_file(
    const std::string &filename) {
  if (std::filesystem::path{filename}.filename().extension().string() !=
      ".txt") {
    error("must be a txt file: {}", filename);
  }

  std::ifstream ifs{filename};
  check_file_is_open(ifs, filename);

  std::vector<std::string> texts;
  std::string line;

  while (std::getline(ifs, line)) {
    auto str{trans_str(line)};
    if (!std::empty(str)) {
      if (!std::empty(texts) && texts.back().ends_with("，")) {
        texts.back().append(str);
      } else {
        texts.push_back(str);
      }
    }
  }

  auto book_name{std::filesystem::path{filename}.filename().stem().string()};
  book_name = trans_str(book_name);

  return {book_name, texts};
}
