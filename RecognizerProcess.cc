
#include "RecognizerProcess.hh"
#include "str.hh"

const unsigned int RecognizerProcess::MIN_BEAM = 1;
const unsigned int RecognizerProcess::MAX_BEAM = 200;
const unsigned int RecognizerProcess::MIN_LMSCALE = 1;
const unsigned int RecognizerProcess::MAX_LMSCALE = 100;

RecognizerProcess::RecognizerProcess(const std::string &computer,
                                   const std::string &script)
{
  this->m_computer = computer;
  this->m_script = script;
  
  // Try to read default settings from the script file.
  if (!this->read_settings()) {
    this->m_beam = MAX_BEAM;
    this->m_lmscale = MAX_LMSCALE;
  }
}

void
RecognizerProcess::start()
{
  // Fork a child process.
  if (this->m_process.create() == 0) {
    // Child process enters here.
    int ret = execlp("ssh",
                     "ssh",
                     this->m_computer.data(),
                     this->m_script.data(),
                     (char*)NULL);
                    
    if (ret < 0) {
      perror("Recognizer process failed exec()");
      exit(1);                                    
    }
    // Child process never gets here.
    assert(false);
  }
  // Parent process continues here.
  
  msg::set_non_blocking(this->m_process.read_fd);
  msg::set_non_blocking(this->m_process.write_fd);
  
  this->m_in_queue.enable(this->m_process.read_fd);
  this->m_out_queue.enable(this->m_process.write_fd);

  this->send_settings();  
}

void
RecognizerProcess::finish()
{
  if (this->m_process.is_created()) {
    this->m_process.finish();
    this->m_in_queue.disable();
    this->m_out_queue.disable();
  }
}

bool
RecognizerProcess::set_computer(const std::string &computer)
{
  if (!this->m_process.is_created()) {
    this->m_computer = computer;
    return true;
  }
  return false;
}

bool
RecognizerProcess::set_script(const std::string &script)
{
  if (!this->m_process.is_created()) {
    this->m_script = script;
    return true;
  }
  return false;
}

bool
RecognizerProcess::set_beam(unsigned int beam)
//throw(msg::ExceptionBrokenPipe)
{
  if (beam < MIN_BEAM || beam > MAX_BEAM)
    return false;

  this->send_parameter("beam", beam);
  this->m_beam = beam;
  return true;
}

bool
RecognizerProcess::set_lmscale(unsigned int lmscale)
//throw(msg::ExceptionBrokenPipe)
{
  if (lmscale < MIN_LMSCALE || lmscale > MAX_LMSCALE)
    return false;

  this->send_parameter("lm_scale", lmscale);
  this->m_lmscale = lmscale;
  return true;
}

void
RecognizerProcess::send_settings()
{
  this->send_parameter("beam", this->m_beam);
  this->send_parameter("lm_scale", this->m_lmscale);
}

void
RecognizerProcess::send_parameter(const std::string &name, unsigned int value)
{
  msg::Message message(msg::M_DECODER_SETTING, true);
  char buffer[20];

  sprintf(buffer, "%s %d", name.data(), value);
  message.append(buffer);
  this->m_out_queue.add_message(message);
}

bool
RecognizerProcess::read_settings()
{
  FILE *file;
  std::string line;
  std::vector<std::string> fields;
  bool read_settings = false;
  bool ok = true;
  unsigned int beam = 0, lmscale = 0;
  
  file = fopen(this->m_script.data(), "r");
  if (!file) {
    fprintf(stderr, "RecognizerProcess::read_settings: Error opening file.\n");
    return false;
  }

  while (!feof(file) && !read_settings) {
    str::read_line(&line, file, true);
    if (line.compare(0, 12, "./recognizer") == 0) {
      str::split(&line, " \"", true, &fields);
      for (unsigned int ind = 0; ind < fields.size() - 1; ind++) {
        if (fields[ind] == "--lm-scale") {
          lmscale = str::str2long(&fields[ind+1], &ok);
        }
        if (fields[ind] == "--beam") {
          beam = str::str2long(&fields[ind+1], &ok);
        }
      }
      read_settings = true;
    }
  }

  fclose(file);
  
  // Check validity.
  ok &= (beam >= MIN_BEAM && beam <= MAX_BEAM) &&
        (lmscale >= MIN_LMSCALE && lmscale <= MAX_LMSCALE);
        
  if (read_settings && ok) {
    this->m_beam = beam;
    this->m_lmscale = lmscale;
  }
  else {
    read_settings = false;
    fprintf(stderr,
            "Recognizer process failed to read settings from file \"%s\"\n",
            this->m_script.data());
  }
  
  return read_settings;
}

