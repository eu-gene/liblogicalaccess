#include "cl1356cardprobe.hpp"
#include "logicalaccess/cards/chip.hpp"
#include "logicalaccess/logs.hpp"
#include "mifarecommands.hpp"
#include "mifareprofile.hpp"

using namespace logicalaccess;

CL1356CardProbe::CL1356CardProbe(ReaderUnit *ru)
    : PCSCCardProbe(ru)
{
}

bool CL1356CardProbe::maybe_mifare_classic()
{
    try
    {
        LLA_LOG_CTX("CL1356CardProbe::maybe_mifare_classic");
        reset();
        auto chip = reader_unit_->createChip("Mifare1K");
        std::shared_ptr<MifareCommands> command =
            std::dynamic_pointer_cast<MifareCommands>(chip->getCommands());

        logicalaccess::MifareAccessInfo::SectorAccessBits sab;
        std::dynamic_pointer_cast<logicalaccess::MifareProfile>(chip->getProfile())
            ->setDefaultKeysAt(0x00);
        auto ret = command->readSector(0, 1, sab);
        return true;
    }
    catch (const CardException &e)
    {
        if (e.error_code() == CardException::ErrorType::UNKOWN_ERROR)
        {
            // The reader returns the generic 0x6F00 signal a generic error
            // when the authentication failed.
            return true;
        }
    }
    catch (const std::exception &)
    {
        // If an error occurred, the card probably isn't mifare classic.
        return false;
    }
    return false;
}
