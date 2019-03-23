#include "Mana.h"

#include "Character.h"
#include "CharacterStats.h"
#include "Engine.h"
#include "ManaTick.h"
#include "ResourceGain.h"
#include "Utils/Check.h"

Mana::Mana(Character* pchar) :
    pchar(pchar),
    base_mana(0),
    mana_per_tick(0)
{
    this->max = get_max_mana();
    this->current = max;
    this->mana_tick = new ManaTick(pchar, this);
}

Mana::~Mana() {
    delete mana_tick;
}

void Mana::set_base_mana(const unsigned base_mana) {
    this->base_mana = base_mana;
    reset_resource();
}

unsigned Mana::get_max_mana() const {
    return base_mana + pchar->get_stats()->get_intellect() * 15;
}

void Mana::gain_resource(const unsigned mana) {
    current += mana;

    if (current > max)
        current = max;
}

void Mana::add_next_tick() {
    auto* event = new ResourceGain(pchar, mana_tick, pchar->get_engine()->get_current_priority() + 5.0);
    pchar->get_engine()->add_event(event);
}

void Mana::tick_mana() {
    if (current == max)
        return;

    gain_resource(mana_per_tick);
    pchar->add_player_reaction_event();

    add_next_tick();
}

void Mana::lose_resource(const unsigned mana) {
    if (current == max)
        add_next_tick();

    check((current >= mana), "Underflow decrease");
    current -= mana;
}

void Mana::reset_resource() {
    max = get_max_mana();
    current = max;
    mana_per_tick = 0;
}
